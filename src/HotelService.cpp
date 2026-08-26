#include "hotel/HotelService.hpp"
#include "hotel/Exceptions.hpp"

#include <chrono>
#include <format>
#include <string>

namespace hotel {

namespace {

/** Renders a stay as "05/10/2026 - 08/10/2026" for log messages. */
std::string formatDate(const DateRange& range) {
    return std::format("{:%d/%m/%Y} - {:%d/%m/%Y}",
                       range.getCheckIn(), range.getCheckOut());
}

} // namespace

HotelService::HotelService(std::unique_ptr<BillingStrategy> billingStrategy)
    : billingStrategy_(std::move(billingStrategy)) {}

void HotelService::addRoom(int roomNumber, std::shared_ptr<RoomCategory> category) {
    rooms_.push_back(std::make_unique<Room>(roomNumber, std::move(category)));
    availabilityIndex_.registerRoom(roomNumber); // must happen before any threading begins
}

Room& HotelService::findRoom(int roomNumber) {
    for (auto& room : rooms_) {
        if (room->getRoomNumber() == roomNumber) {
            return *room;
        }
    }
    throw RoomNotFoundException("Room not found: " + std::to_string(roomNumber));
}

Reservation& HotelService::findReservationUnlocked(int reservationId) {
    auto it = reservations_.find(reservationId);
    if (it == reservations_.end()) {
        throw InvalidReservationException("Reservation not found: " + std::to_string(reservationId));
    }
    return it->second;
}

Reservation HotelService::bookRoom(const std::string& categoryName, const DateRange& dates, Guest guest) {
    for (auto& room : rooms_) {
        if (room->getCategory()->getName() != categoryName) {
            continue;
        }

        // tryBook() is the atomic check-and-book — the fix for the classic
        // "check, then act" race between concurrent booking threads.
        if (availabilityIndex_.tryBook(room->getRoomNumber(), dates)) {
            int id = nextReservationId_++; // atomic increment — safe across threads
            Reservation reservation(id, room->getRoomNumber(), std::move(guest), dates);

            {
                std::lock_guard<std::mutex> lock(reservationsMutex_);
                reservations_.emplace(id, reservation);
            }

            logger_.log("Booking created: reservation " + std::to_string(id) +
                        " room " + std::to_string(room->getRoomNumber()));

            return reservation;
        }
    }

    throw RoomUnavailableException("No available room of category: " + categoryName);
}

void HotelService::checkIn(int reservationId) {
    std::lock_guard<std::mutex> lock(reservationsMutex_);
    Reservation& reservation = findReservationUnlocked(reservationId);

    if (reservation.getStatus() != ReservationStatus::Booked) {
        throw InvalidReservationException("Reservation is not in Booked state, cannot check in");
    }

    Room& room = findRoom(reservation.getRoomNumber());
    room.setStatus(RoomStatus::Occupied);
    reservation.setStatus(ReservationStatus::CheckedIn);

    logger_.log("Checked in: reservation " + std::to_string(reservationId));
}

double HotelService::checkOut(int reservationId, double serviceCharges) {
    std::lock_guard<std::mutex> lock(reservationsMutex_);
    Reservation& reservation = findReservationUnlocked(reservationId);

    if (reservation.getStatus() != ReservationStatus::CheckedIn) {
        throw InvalidReservationException("Reservation is not in CheckedIn state, cannot check out");
    }

    Room& room = findRoom(reservation.getRoomNumber());

    auto nights = (std::chrono::sys_days{reservation.getDates().getCheckOut()} -
                   std::chrono::sys_days{reservation.getDates().getCheckIn()}).count();

    double rate = room.getCategory()->getBaseRate();
    double bill = billingStrategy_->calculateBill(static_cast<int>(nights), rate, serviceCharges);

    room.setStatus(RoomStatus::Vacant);
    reservation.setStatus(ReservationStatus::CheckedOut);

    totalRevenue_ += bill;
    totalNightsCheckedOut_ += nights;
    checkoutCount_ += 1;

    logger_.log("Checked out: reservation " + std::to_string(reservationId) +
                " bill " + std::to_string(bill));

    return bill;
}

void HotelService::modifyReservation(int reservationId, const DateRange& newDates) {
    std::lock_guard<std::mutex> lock(reservationsMutex_);
    Reservation& reservation = findReservationUnlocked(reservationId);

    if (reservation.getStatus() != ReservationStatus::Booked) {
        throw InvalidReservationException(
            "Only a Booked reservation can be modified: reservation " +
            std::to_string(reservationId) + " has already been checked in, "
            "checked out or cancelled");
    }

    const DateRange oldDates = reservation.getDates();
    const int roomNumber = reservation.getRoomNumber();

    // One atomic swap on this room's lock. If it fails, the index still holds
    // the original dates, so throwing here leaves the booking untouched.
    if (!availabilityIndex_.tryReplace(roomNumber, oldDates, newDates)) {
        throw RoomUnavailableException(
            "Room " + std::to_string(roomNumber) + " is not free for the new dates; "
            "reservation " + std::to_string(reservationId) + " is unchanged");
    }

    reservation.setDates(newDates);

    logger_.log("Modified: reservation " + std::to_string(reservationId) +
                " room " + std::to_string(roomNumber) +
                " moved from " + formatDate(oldDates) + " to " + formatDate(newDates));
}

void HotelService::cancelReservation(int reservationId) {
    std::lock_guard<std::mutex> lock(reservationsMutex_);
    Reservation& reservation = findReservationUnlocked(reservationId);

    if (reservation.getStatus() != ReservationStatus::Booked) {
        throw InvalidReservationException("Only a Booked reservation can be cancelled");
    }

    availabilityIndex_.markFreed(reservation.getRoomNumber(), reservation.getDates());
    reservation.setStatus(ReservationStatus::Cancelled);

    logger_.log("Cancelled: reservation " + std::to_string(reservationId));
}

std::vector<Room*> HotelService::searchRooms(const std::string& categoryName, const DateRange& dates) {
    std::vector<Room*> result;
    for (auto& room : rooms_) {
        if (room->getCategory()->getName() == categoryName &&
            availabilityIndex_.isAvailable(room->getRoomNumber(), dates)) {
            result.push_back(room.get());
        }
    }
    return result;
}

Report HotelService::getReports() const {
    std::lock_guard<std::mutex> lock(reservationsMutex_);

    Report report;
    std::map<std::string, int> totalByCategory;
    std::map<std::string, int> occupiedByCategory;

    for (const auto& room : rooms_) {
        const std::string& name = room->getCategory()->getName();
        totalByCategory[name]++;
        if (room->getStatus() == RoomStatus::Occupied) {
            occupiedByCategory[name]++;
        }
    }

    for (const auto& [category, total] : totalByCategory) {
        int occupied = occupiedByCategory.count(category) ? occupiedByCategory[category] : 0;
        double rate = total > 0 ? static_cast<double>(occupied) / total : 0.0;
        report.occupancyByCategory.push_back({category, rate, total, occupied});
    }

    // Count reservations by status so the report shows booking activity,
    // not just guests who have already checked out.
    for (const auto& [id, reservation] : reservations_) {
        switch (reservation.getStatus()) {
            case ReservationStatus::Booked:      report.bookedAwaitingCheckIn++; break;
            case ReservationStatus::CheckedIn:   report.currentlyCheckedIn++;    break;
            case ReservationStatus::CheckedOut:  report.completedStays++;        break;
            case ReservationStatus::Cancelled:   report.cancelled++;             break;
        }
    }

    report.totalRevenue = totalRevenue_;
    report.averageStayLengthNights =
        checkoutCount_ > 0 ? static_cast<double>(totalNightsCheckedOut_) / checkoutCount_ : 0.0;

    return report;
}

}
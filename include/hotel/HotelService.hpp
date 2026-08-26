#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "hotel/AvailabilityIndex.hpp"
#include "hotel/BillingStrategy.hpp"
#include "hotel/DateRange.hpp"
#include "hotel/Guest.hpp"
#include "hotel/Logger.hpp"
#include "hotel/Reservation.hpp"
#include "hotel/Room.hpp"
#include "hotel/RoomCategory.hpp"

namespace hotel {

struct CategoryOccupancy {
    std::string category;
    double occupancyRate;   // occupiedRooms / totalRooms, 0.0 to 1.0
    int totalRooms = 0;
    int occupiedRooms = 0;
};

struct Report {
    std::vector<CategoryOccupancy> occupancyByCategory;
    double totalRevenue = 0.0;
    double averageStayLengthNights = 0.0;

    // Reservation counts by status. Without these the report reads as all
    // zeros until somebody actually checks out, which makes a working
    // system look broken.
    int bookedAwaitingCheckIn = 0;
    int currentlyCheckedIn = 0;
    int completedStays = 0;
    int cancelled = 0;
};

/**
 * Top-level facade. Owns the room inventory, AvailabilityIndex,
 * BillingStrategy, and Logger, and coordinates every operation.
 *
 * Thread safety: bookRoom() is safe to call concurrently from multiple
 * threads (per-room locking in AvailabilityIndex + a mutex guarding the
 * shared reservations map/ID counter). checkIn/checkOut/cancelReservation
 * lock the same reservations mutex, so they're safe with respect to each
 * other and to concurrent bookings too. Room.setStatus() itself is a
 * simple field write — fine for this assignment's concurrent-BOOKING
 * requirement, though a fully hardened production system would protect
 * per-room status writes as well.
 */
class HotelService {
public:
    explicit HotelService(std::unique_ptr<BillingStrategy> billingStrategy);

    void addRoom(int roomNumber, std::shared_ptr<RoomCategory> category);

    Reservation bookRoom(const std::string& categoryName, const DateRange& dates, Guest guest);

    void checkIn(int reservationId);
    double checkOut(int reservationId, double serviceCharges = 0.0);
    void cancelReservation(int reservationId);

    std::vector<Room*> searchRooms(const std::string& categoryName, const DateRange& dates);

    Report getReports() const;

    Room& findRoom(int roomNumber);

private:
    Reservation& findReservationUnlocked(int reservationId);

    std::vector<std::unique_ptr<Room>> rooms_;
    AvailabilityIndex availabilityIndex_;
    std::unique_ptr<BillingStrategy> billingStrategy_;
    Logger logger_;

    std::map<int, Reservation> reservations_;
    std::atomic<int> nextReservationId_{1};
    mutable std::mutex reservationsMutex_;

    double totalRevenue_ = 0.0;
    long long totalNightsCheckedOut_ = 0;
    int checkoutCount_ = 0;
};

} 
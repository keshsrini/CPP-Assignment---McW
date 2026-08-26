#include "hotel/Reservation.hpp"

namespace hotel {

Reservation::Reservation(int reservationId, int roomNumber, Guest guest, DateRange dates)
    : reservationId_(reservationId),
      roomNumber_(roomNumber),
      guest_(std::move(guest)),
      dates_(dates),
      status_(ReservationStatus::Booked) {}

int Reservation::getReservationId() const { return reservationId_; }
int Reservation::getRoomNumber() const { return roomNumber_; }
const Guest& Reservation::getGuest() const { return guest_; }
const DateRange& Reservation::getDates() const { return dates_; }
ReservationStatus Reservation::getStatus() const { return status_; }

void Reservation::setStatus(ReservationStatus status) { status_ = status; }

void Reservation::setDates(const DateRange& dates) { dates_ = dates; }

}
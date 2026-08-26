#pragma once

#include "hotel/DateRange.hpp"
#include "hotel/Guest.hpp"

namespace hotel {

enum class ReservationStatus {
    Booked,
    CheckedIn,
    CheckedOut,
    Cancelled
};

class Reservation {
public:
    Reservation(int reservationId, int roomNumber, Guest guest, DateRange dates);

    int getReservationId() const;
    int getRoomNumber() const;
    const Guest& getGuest() const;
    const DateRange& getDates() const;
    ReservationStatus getStatus() const;

    void setStatus(ReservationStatus status);
    void setDates(const DateRange& dates);

private:
    int reservationId_;
    int roomNumber_;
    Guest guest_;
    DateRange dates_;
    ReservationStatus status_;
};

}
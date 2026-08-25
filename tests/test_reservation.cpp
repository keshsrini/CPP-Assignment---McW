#include <gtest/gtest.h>
#include "hotel/Reservation.hpp"

using namespace std::chrono;
using hotel::Reservation;
using hotel::ReservationStatus;
using hotel::Guest;
using hotel::DateRange;

TEST(ReservationTest, NewReservationDefaultsToBooked) {
    Guest guest("G001", "Keshav", "keshav@example.com");
    DateRange dates(2026y/October/5d, 2026y/October/8d);
    Reservation res(1, 101, guest, dates);
    EXPECT_EQ(res.getStatus(), ReservationStatus::Booked);
}

TEST(ReservationTest, StoresIdsAndDatesCorrectly) {
    Guest guest("G002", "Anita", "anita@example.com");
    DateRange dates(2026y/November/1d, 2026y/November/4d);
    Reservation res(42, 205, guest, dates);
    EXPECT_EQ(res.getReservationId(), 42);
    EXPECT_EQ(res.getRoomNumber(), 205);
    EXPECT_EQ(res.getDates().getCheckIn(), 2026y/November/1d);
}

TEST(ReservationTest, StoresGuestCorrectly) {
    Guest guest("G003", "Ravi", "ravi@example.com");
    DateRange dates(2026y/October/1d, 2026y/October/3d);
    Reservation res(2, 102, guest, dates);
    EXPECT_EQ(res.getGuest().getName(), "Ravi");
    EXPECT_EQ(res.getGuest().getGuestId(), "G003");
}

TEST(ReservationTest, SetStatusTransitionsCorrectly) {
    Guest guest("G004", "Priya", "priya@example.com");
    DateRange dates(2026y/October/1d, 2026y/October/3d);
    Reservation res(3, 103, guest, dates);
    res.setStatus(ReservationStatus::CheckedIn);
    EXPECT_EQ(res.getStatus(), ReservationStatus::CheckedIn);
    res.setStatus(ReservationStatus::CheckedOut);
    EXPECT_EQ(res.getStatus(), ReservationStatus::CheckedOut);
}

TEST(ReservationTest, CancelledStatusStoredCorrectly) {
    Guest guest("G005", "Vikram", "vikram@example.com");
    DateRange dates(2026y/December/1d, 2026y/December/5d);
    Reservation res(4, 104, guest, dates);
    res.setStatus(ReservationStatus::Cancelled);
    EXPECT_EQ(res.getStatus(), ReservationStatus::Cancelled);
}
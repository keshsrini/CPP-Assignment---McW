#include <gtest/gtest.h>
#include "hotel/HotelService.hpp"
#include "hotel/Exceptions.hpp"

using namespace std::chrono;
using namespace hotel;

namespace {

std::unique_ptr<HotelService> makeServiceWithRooms() {
    auto service = std::make_unique<HotelService>(std::make_unique<StandardBillingStrategy>());

    auto standard = std::make_shared<RoomCategory>("Standard", 2, 2000.0, std::vector<std::string>{"WiFi"});
    auto deluxe = std::make_shared<RoomCategory>("Deluxe", 3, 4000.0, std::vector<std::string>{"WiFi", "Mini Bar"});

    service->addRoom(101, standard);
    service->addRoom(102, standard);
    service->addRoom(201, deluxe);

    return service;
}

}

TEST(HotelServiceTest, BookRoomSucceedsForAvailableCategory) {
    auto service = makeServiceWithRooms();
    DateRange dates(2026y/October/5d, 2026y/October/8d);

    Reservation res = service->bookRoom("Standard", dates, Guest("G1", "Keshav", "keshav@example.com"));

    EXPECT_EQ(res.getStatus(), ReservationStatus::Booked);
    EXPECT_EQ(res.getGuest().getName(), "Keshav");
}

TEST(HotelServiceTest, BookRoomThrowsWhenCategoryFullyBooked) {
    auto service = makeServiceWithRooms();
    DateRange dates(2026y/October/5d, 2026y/October/8d);

    service->bookRoom("Standard", dates, Guest("G1", "A", "a@example.com"));
    service->bookRoom("Standard", dates, Guest("G2", "B", "b@example.com"));

    EXPECT_THROW(
        service->bookRoom("Standard", dates, Guest("G3", "C", "c@example.com")),
        RoomUnavailableException
    );
}

TEST(HotelServiceTest, CheckInMarksRoomStatusOccupied) {
    auto service = makeServiceWithRooms();
    DateRange dates(2026y/October/5d, 2026y/October/8d);
    Reservation res = service->bookRoom("Standard", dates, Guest("G1", "A", "a@example.com"));

    service->checkIn(res.getReservationId());

    Room& room = service->findRoom(res.getRoomNumber());
    EXPECT_EQ(room.getStatus(), RoomStatus::Occupied);
}

TEST(HotelServiceTest, CheckOutProducesCorrectBillAndSetsRoomVacant) {
    auto service = makeServiceWithRooms();
    DateRange dates(2026y/October/5d, 2026y/October/8d); // 3 nights
    Reservation res = service->bookRoom("Standard", dates, Guest("G1", "A", "a@example.com"));

    service->checkIn(res.getReservationId());
    double bill = service->checkOut(res.getReservationId());

    EXPECT_DOUBLE_EQ(bill, 6000.0); 

    Room& room = service->findRoom(res.getRoomNumber());
    EXPECT_EQ(room.getStatus(), RoomStatus::Vacant);
}

TEST(HotelServiceTest, CancelBeforeCheckInFreesAvailability) {
    auto service = makeServiceWithRooms(); 
    DateRange dates(2026y/October/5d, 2026y/October/8d);

    Reservation res1 = service->bookRoom("Standard", dates, Guest("G1", "A", "a@example.com"));
    service->bookRoom("Standard", dates, Guest("G2", "B", "b@example.com"));

    EXPECT_THROW(
        service->bookRoom("Standard", dates, Guest("G3", "C", "c@example.com")),
        RoomUnavailableException
    );

    service->cancelReservation(res1.getReservationId());

    EXPECT_NO_THROW(service->bookRoom("Standard", dates, Guest("G4", "D", "d@example.com")));
}

TEST(HotelServiceTest, CancelAfterCheckInThrows) {
    auto service = makeServiceWithRooms();
    DateRange dates(2026y/October/5d, 2026y/October/8d);
    Reservation res = service->bookRoom("Standard", dates, Guest("G1", "A", "a@example.com"));

    service->checkIn(res.getReservationId());

    EXPECT_THROW(service->cancelReservation(res.getReservationId()), InvalidReservationException);
}

TEST(HotelServiceTest, SearchRoomsReturnsOnlyMatchingCategoryAndAvailability) {
    auto service = makeServiceWithRooms();
    DateRange dates(2026y/October/5d, 2026y/October/8d);

    auto results = service->searchRooms("Deluxe", dates);

    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0]->getCategory()->getName(), "Deluxe");
}

TEST(HotelServiceTest, GetReportsReflectsRevenueAfterCheckout) {
    auto service = makeServiceWithRooms();
    DateRange dates(2026y/October/5d, 2026y/October/8d); 
    Reservation res = service->bookRoom("Standard", dates, Guest("G1", "A", "a@example.com"));
    service->checkIn(res.getReservationId());
    service->checkOut(res.getReservationId());

    Report report = service->getReports();

    EXPECT_DOUBLE_EQ(report.totalRevenue, 6000.0);
    EXPECT_DOUBLE_EQ(report.averageStayLengthNights, 3.0);
}

TEST(HotelServiceTest, CheckInThrowsForNonexistentReservation) {
    auto service = makeServiceWithRooms();
    EXPECT_THROW(service->checkIn(9999), InvalidReservationException);
}

// --- modifyReservation -----------------------------------------------------

// Billing reads the reservation's dates at check-out, so a correct bill for the
// NEW length proves the stored dates really moved, not just the index.
TEST(HotelServiceTest, ModifyReservationChangesStayLengthAndBill) {
    auto service = makeServiceWithRooms();
    DateRange original(2026y/October/5d, 2026y/October/8d);   // 3 nights = 6000
    Reservation res = service->bookRoom("Standard", original,
                                        Guest("G1", "A", "a@example.com"));

    DateRange longer(2026y/October/5d, 2026y/October/10d);    // 5 nights = 10000
    service->modifyReservation(res.getReservationId(), longer);

    service->checkIn(res.getReservationId());
    double bill = service->checkOut(res.getReservationId());

    EXPECT_DOUBLE_EQ(bill, 10000.0);
}

TEST(HotelServiceTest, ModifyReservationReleasesTheOldDates) {
    auto service = makeServiceWithRooms();
    DateRange original(2026y/October/5d, 2026y/October/8d);

    // Fill both Standard rooms so the category is full for those dates.
    Reservation res1 = service->bookRoom("Standard", original, Guest("G1", "A", "a@example.com"));
    service->bookRoom("Standard", original, Guest("G2", "B", "b@example.com"));
    EXPECT_THROW(service->bookRoom("Standard", original, Guest("G3", "C", "c@example.com")),
                 RoomUnavailableException);

    // Move one booking well away; its old dates should free up.
    DateRange moved(2026y/December/1d, 2026y/December/4d);
    service->modifyReservation(res1.getReservationId(), moved);

    EXPECT_NO_THROW(service->bookRoom("Standard", original, Guest("G4", "D", "d@example.com")));
}

// Shifting a stay by one night overlaps its own existing booking.
TEST(HotelServiceTest, ModifyReservationCanShiftDatesOntoOverlappingRange) {
    auto service = makeServiceWithRooms();
    DateRange original(2026y/October/5d, 2026y/October/8d);
    Reservation res = service->bookRoom("Standard", original,
                                        Guest("G1", "A", "a@example.com"));

    DateRange shifted(2026y/October/6d, 2026y/October/9d);
    EXPECT_NO_THROW(service->modifyReservation(res.getReservationId(), shifted));
}

TEST(HotelServiceTest, ModifyReservationThrowsWhenNewDatesTakenAndKeepsOriginal) {
    auto service = makeServiceWithRooms();

    // Both bookings land on room 101 - same room, non-overlapping dates.
    DateRange early(2026y/October/5d, 2026y/October/8d);
    DateRange late(2026y/October/12d, 2026y/October/15d);
    Reservation res1 = service->bookRoom("Standard", early, Guest("G1", "A", "a@example.com"));
    Reservation res2 = service->bookRoom("Standard", late, Guest("G2", "B", "b@example.com"));
    ASSERT_EQ(res1.getRoomNumber(), res2.getRoomNumber());

    // Moving res1 onto res2's dates must fail.
    DateRange clashing(2026y/October/13d, 2026y/October/14d);
    EXPECT_THROW(service->modifyReservation(res1.getReservationId(), clashing),
                 RoomUnavailableException);

    // res1 must still hold its original 3 nights, so the bill is unchanged.
    service->checkIn(res1.getReservationId());
    EXPECT_DOUBLE_EQ(service->checkOut(res1.getReservationId()), 6000.0);
}

TEST(HotelServiceTest, ModifyReservationThrowsAfterCheckIn) {
    auto service = makeServiceWithRooms();
    DateRange dates(2026y/October/5d, 2026y/October/8d);
    Reservation res = service->bookRoom("Standard", dates, Guest("G1", "A", "a@example.com"));
    service->checkIn(res.getReservationId());

    DateRange moved(2026y/October/20d, 2026y/October/23d);
    EXPECT_THROW(service->modifyReservation(res.getReservationId(), moved),
                 InvalidReservationException);
}

TEST(HotelServiceTest, ModifyReservationThrowsAfterCancellation) {
    auto service = makeServiceWithRooms();
    DateRange dates(2026y/October/5d, 2026y/October/8d);
    Reservation res = service->bookRoom("Standard", dates, Guest("G1", "A", "a@example.com"));
    service->cancelReservation(res.getReservationId());

    DateRange moved(2026y/October/20d, 2026y/October/23d);
    EXPECT_THROW(service->modifyReservation(res.getReservationId(), moved),
                 InvalidReservationException);
}

TEST(HotelServiceTest, ModifyReservationThrowsForUnknownId) {
    auto service = makeServiceWithRooms();
    DateRange dates(2026y/October/5d, 2026y/October/8d);

    EXPECT_THROW(service->modifyReservation(4242, dates), InvalidReservationException);
}

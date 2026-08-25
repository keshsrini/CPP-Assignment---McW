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
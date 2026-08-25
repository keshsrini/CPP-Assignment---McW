#include <gtest/gtest.h>
#include "hotel/Room.hpp"
#include "hotel/RoomCategory.hpp"

using hotel::Room;
using hotel::RoomCategory;
using hotel::RoomStatus;

TEST(RoomTest, NewRoomDefaultsToVacant) {
    auto deluxe = std::make_shared<RoomCategory>("Deluxe", 3, 4500.0,
                                                   std::vector<std::string>{"WiFi"});
    Room room(101, deluxe);

    EXPECT_EQ(room.getStatus(), RoomStatus::Vacant);
}

TEST(RoomTest, RoomNumberStoredCorrectly) {
    auto standard = std::make_shared<RoomCategory>("Standard", 2, 2000.0,
                                                     std::vector<std::string>{});
    Room room(305, standard);

    EXPECT_EQ(room.getRoomNumber(), 305);
}

TEST(RoomTest, SetStatusChangesStatus) {
    auto suite = std::make_shared<RoomCategory>("Suite", 4, 8000.0,
                                                  std::vector<std::string>{"Jacuzzi"});
    Room room(201, suite);

    room.setStatus(RoomStatus::Occupied);
    EXPECT_EQ(room.getStatus(), RoomStatus::Occupied);

    room.setStatus(RoomStatus::Vacant);
    EXPECT_EQ(room.getStatus(), RoomStatus::Vacant);
}

TEST(RoomTest, RoomHoldsCorrectCategory) {
    auto deluxe = std::make_shared<RoomCategory>("Deluxe", 3, 4500.0,
                                                   std::vector<std::string>{"WiFi"});
    Room room(102, deluxe);

    EXPECT_EQ(room.getCategory()->getName(), "Deluxe");
    EXPECT_DOUBLE_EQ(room.getCategory()->getBaseRate(), 4500.0);
}

TEST(RoomTest, MultipleRoomsShareSameCategoryInstance) {
    auto deluxe = std::make_shared<RoomCategory>("Deluxe", 3, 4500.0,
                                                   std::vector<std::string>{"WiFi"});
    Room room1(103, deluxe);
    Room room2(104, deluxe);

    EXPECT_EQ(room1.getCategory().get(), room2.getCategory().get());
}
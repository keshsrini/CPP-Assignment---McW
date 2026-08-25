#include "hotel/Room.hpp"

namespace hotel {

Room::Room(int roomNumber, std::shared_ptr<RoomCategory> category)
    : roomNumber_(roomNumber),
      category_(std::move(category)),
      status_(RoomStatus::Vacant) {}

int Room::getRoomNumber() const { return roomNumber_; }

const std::shared_ptr<RoomCategory>& Room::getCategory() const { return category_; }

RoomStatus Room::getStatus() const { return status_; }

void Room::setStatus(RoomStatus status) { status_ = status; }

} 
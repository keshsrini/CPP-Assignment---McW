#pragma once

#include <memory>
#include "hotel/RoomCategory.hpp"

namespace hotel {

enum class RoomStatus {
    Vacant,
    Occupied,
    UnderMaintenance
};

class Room {
public:
    Room(int roomNumber, std::shared_ptr<RoomCategory> category);

    int getRoomNumber() const;
    const std::shared_ptr<RoomCategory>& getCategory() const;
    RoomStatus getStatus() const;

    void setStatus(RoomStatus status);

private:
    int roomNumber_;
    std::shared_ptr<RoomCategory> category_;
    RoomStatus status_;
};

}
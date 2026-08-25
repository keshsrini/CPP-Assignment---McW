#include "hotel/RoomCategory.hpp"

namespace hotel {

RoomCategory::RoomCategory(std::string name,
                            int capacity,
                            double baseRate,
                            std::vector<std::string> amenities)
    : name_(std::move(name)),
      capacity_(capacity),
      baseRate_(baseRate),
      amenities_(std::move(amenities)) {}

const std::string& RoomCategory::getName() const {
    return name_;
}

int RoomCategory::getCapacity() const {
    return capacity_;
}

double RoomCategory::getBaseRate() const {
    return baseRate_;
}

const std::vector<std::string>& RoomCategory::getAmenities() const {
    return amenities_;
}

}
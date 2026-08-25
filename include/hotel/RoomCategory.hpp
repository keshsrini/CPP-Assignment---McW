#pragma once
#include <string>
#include <vector>

namespace hotel {
class RoomCategory {
public:
    RoomCategory(std::string name,
                 int capacity,
                 double baseRate,
                 std::vector<std::string> amenities);

    const std::string& getName() const;
    int getCapacity() const;
    double getBaseRate() const;
    const std::vector<std::string>& getAmenities() const;

private:
    std::string name_;
    int capacity_;
    double baseRate_;
    std::vector<std::string> amenities_;
};

}
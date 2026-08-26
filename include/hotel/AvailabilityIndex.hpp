#pragma once

#include <map>
#include <mutex>
#include <vector>
#include "hotel/DateRange.hpp"

namespace hotel {

class AvailabilityIndex {
public:
    void registerRoom(int roomNumber);
    bool isAvailable(int roomNumber, const DateRange& range) const;
    void markBooked(int roomNumber, const DateRange& range);
    void markFreed(int roomNumber, const DateRange& range);
    bool tryBook(int roomNumber, const DateRange& range);
    bool tryReplace(int roomNumber, const DateRange& oldRange, const DateRange& newRange);

private:
    bool isAvailableUnlocked(int roomNumber, const DateRange& range) const;
    void markBookedUnlocked(int roomNumber, const DateRange& range);
    void removeRangeUnlocked(int roomNumber, const DateRange& range);

    std::map<int, std::vector<DateRange>> bookedRanges_;
    mutable std::map<int, std::mutex> roomMutexes_;
};

} 
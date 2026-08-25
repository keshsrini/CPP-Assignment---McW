#include "hotel/AvailabilityIndex.hpp"
#include <algorithm>

namespace hotel {

void AvailabilityIndex::registerRoom(int roomNumber) {
    bookedRanges_[roomNumber];
    roomMutexes_[roomNumber];    
}

bool AvailabilityIndex::isAvailableUnlocked(int roomNumber, const DateRange& range) const {
    const auto& ranges = bookedRanges_.at(roomNumber);
    for (const auto& existing : ranges) {
        if (existing.overlaps(range)) {
            return false;
        }
    }
    return true;
}

void AvailabilityIndex::markBookedUnlocked(int roomNumber, const DateRange& range) {
    bookedRanges_.at(roomNumber).push_back(range);
}

bool AvailabilityIndex::isAvailable(int roomNumber, const DateRange& range) const {
    std::lock_guard<std::mutex> lock(roomMutexes_.at(roomNumber));
    return isAvailableUnlocked(roomNumber, range);
}

void AvailabilityIndex::markBooked(int roomNumber, const DateRange& range) {
    std::lock_guard<std::mutex> lock(roomMutexes_.at(roomNumber));
    markBookedUnlocked(roomNumber, range);
}

void AvailabilityIndex::markFreed(int roomNumber, const DateRange& range) {
    std::lock_guard<std::mutex> lock(roomMutexes_.at(roomNumber));
    auto& ranges = bookedRanges_.at(roomNumber);
    ranges.erase(
        std::remove_if(ranges.begin(), ranges.end(),
            [&range](const DateRange& existing) {
                return existing.getCheckIn() == range.getCheckIn() &&
                       existing.getCheckOut() == range.getCheckOut();
            }),
        ranges.end()
    );
}

bool AvailabilityIndex::tryBook(int roomNumber, const DateRange& range) {
    std::lock_guard<std::mutex> lock(roomMutexes_.at(roomNumber));
    if (!isAvailableUnlocked(roomNumber, range)) {
        return false;
    }
    markBookedUnlocked(roomNumber, range);
    return true;
}

} 
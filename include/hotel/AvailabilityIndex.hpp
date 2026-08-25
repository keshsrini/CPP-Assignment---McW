#pragma once

#include <map>
#include <mutex>
#include <vector>
#include "hotel/DateRange.hpp"

namespace hotel {

/**
 * Tracks which date ranges are booked for each room, and answers
 * availability queries. Thread-safe via one mutex PER ROOM — booking
 * Room 101 never blocks a concurrent booking of Room 305, since they
 * protect independent data.
 *
 * IMPORTANT: registerRoom() must be called for every room during setup,
 * BEFORE any threads start booking. This avoids the "two threads racing
 * to create the same room's mutex for the first time" problem entirely —
 * after setup, we only ever look up existing mutexes, never insert new
 * ones concurrently.
 */
class AvailabilityIndex {
public:
    /** Registers a room so it has its own lock and booking record.
     *  Call only during single-threaded setup. */
    void registerRoom(int roomNumber);

    bool isAvailable(int roomNumber, const DateRange& range) const;
    void markBooked(int roomNumber, const DateRange& range);
    void markFreed(int roomNumber, const DateRange& range);

    /**
     * Atomically checks availability and books, under ONE lock acquisition.
     * Returns true if booked, false if the room was already unavailable.
     * This is what makes concurrent booking safe: calling isAvailable()
     * and markBooked() as two separate calls leaves a gap where another
     * thread could book in between — tryBook() has no such gap.
     */
    bool tryBook(int roomNumber, const DateRange& range);

private:
    bool isAvailableUnlocked(int roomNumber, const DateRange& range) const;
    void markBookedUnlocked(int roomNumber, const DateRange& range);

    std::map<int, std::vector<DateRange>> bookedRanges_;
    mutable std::map<int, std::mutex> roomMutexes_;
};

} // namespace hotel
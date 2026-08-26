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

    /**
     * Atomically moves an existing booking from oldRange to newRange on the
     * same room, under ONE lock acquisition. Returns true on success, false
     * if newRange clashes with some OTHER booking on that room — in which
     * case oldRange is put back and nothing has changed.
     *
     * Why this can't be done with markFreed() + tryBook(): those are two
     * separate lock acquisitions, so between them the room sits with neither
     * range held and a concurrent bookRoom() could steal the freed dates,
     * leaving the guest with no booking at all if the new dates then fail.
     *
     * Removing oldRange BEFORE testing newRange is also what lets a booking
     * shift by a night or two. Moving 5-8 Oct to 6-9 Oct overlaps itself, so
     * testing first would always report a clash with its own booking.
     */
    bool tryReplace(int roomNumber, const DateRange& oldRange, const DateRange& newRange);

private:
    bool isAvailableUnlocked(int roomNumber, const DateRange& range) const;
    void markBookedUnlocked(int roomNumber, const DateRange& range);
    void removeRangeUnlocked(int roomNumber, const DateRange& range);

    std::map<int, std::vector<DateRange>> bookedRanges_;
    mutable std::map<int, std::mutex> roomMutexes_;
};

} // namespace hotel
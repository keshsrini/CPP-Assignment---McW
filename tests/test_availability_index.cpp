#include <gtest/gtest.h>
#include "hotel/AvailabilityIndex.hpp"

using namespace std::chrono;
using hotel::AvailabilityIndex;
using hotel::DateRange;

TEST(AvailabilityIndexTest, EmptyIndexReportsRoomAsAvailable) {
    AvailabilityIndex index;
    index.registerRoom(101);
    DateRange range(2026y/October/5d, 2026y/October/8d);

    EXPECT_TRUE(index.isAvailable(101, range));
}

TEST(AvailabilityIndexTest, BookedRangeIsNoLongerAvailable) {
    AvailabilityIndex index;
    index.registerRoom(101);
    DateRange range(2026y/October/5d, 2026y/October/8d);

    index.markBooked(101, range);

    EXPECT_FALSE(index.isAvailable(101, range));
}

TEST(AvailabilityIndexTest, OverlappingRangeIsUnavailable) {
    AvailabilityIndex index;
    index.registerRoom(101);
    index.markBooked(101, DateRange(2026y/October/5d, 2026y/October/10d));

    DateRange overlapping(2026y/October/8d, 2026y/October/12d);
    EXPECT_FALSE(index.isAvailable(101, overlapping));
}

TEST(AvailabilityIndexTest, NonOverlappingRangeIsAvailable) {
    AvailabilityIndex index;
    index.registerRoom(101);
    index.markBooked(101, DateRange(2026y/October/1d, 2026y/October/5d));

    DateRange separate(2026y/October/10d, 2026y/October/15d);
    EXPECT_TRUE(index.isAvailable(101, separate));
}

TEST(AvailabilityIndexTest, AdjacentRangeIsAvailable) {
    AvailabilityIndex index;
    index.registerRoom(101);
    index.markBooked(101, DateRange(2026y/October/5d, 2026y/October/10d));

    DateRange adjacent(2026y/October/10d, 2026y/October/15d);
    EXPECT_TRUE(index.isAvailable(101, adjacent));
}

TEST(AvailabilityIndexTest, DifferentRoomsAreIndependent) {
    AvailabilityIndex index;
    index.registerRoom(101);
    index.registerRoom(102);
    index.markBooked(101, DateRange(2026y/October/5d, 2026y/October/10d));

    DateRange sameRange(2026y/October/5d, 2026y/October/10d);
    EXPECT_TRUE(index.isAvailable(102, sameRange));
}

TEST(AvailabilityIndexTest, MarkFreedRestoresAvailability) {
    AvailabilityIndex index;
    index.registerRoom(101);
    DateRange range(2026y/October/5d, 2026y/October/10d);

    index.markBooked(101, range);
    EXPECT_FALSE(index.isAvailable(101, range));

    index.markFreed(101, range);
    EXPECT_TRUE(index.isAvailable(101, range));
}

TEST(AvailabilityIndexTest, MultipleBookingsForSameRoomTrackedCorrectly) {
    AvailabilityIndex index;
    index.registerRoom(101);
    index.markBooked(101, DateRange(2026y/October/1d, 2026y/October/5d));
    index.markBooked(101, DateRange(2026y/October/10d, 2026y/October/15d));

    DateRange gap(2026y/October/5d, 2026y/October/10d);
    EXPECT_TRUE(index.isAvailable(101, gap));

    DateRange overlapsFirst(2026y/October/3d, 2026y/October/7d);
    EXPECT_FALSE(index.isAvailable(101, overlapsFirst));
}

TEST(AvailabilityIndexTest, TryBookSucceedsWhenAvailable) {
    AvailabilityIndex index;
    index.registerRoom(101);
    DateRange range(2026y/October/5d, 2026y/October/8d);

    EXPECT_TRUE(index.tryBook(101, range));
    EXPECT_FALSE(index.isAvailable(101, range)); // now booked
}

TEST(AvailabilityIndexTest, TryBookFailsWhenAlreadyBooked) {
    AvailabilityIndex index;
    index.registerRoom(101);
    DateRange range(2026y/October/5d, 2026y/October/8d);

    EXPECT_TRUE(index.tryBook(101, range));
    EXPECT_FALSE(index.tryBook(101, range)); // second attempt, same range
}

TEST(AvailabilityIndexTest, TryReplaceMovesBookingToNewDates) {
    AvailabilityIndex index;
    index.registerRoom(101);
    DateRange original(2026y/October/5d, 2026y/October/8d);
    DateRange moved(2026y/October/20d, 2026y/October/23d);

    ASSERT_TRUE(index.tryBook(101, original));
    EXPECT_TRUE(index.tryReplace(101, original, moved));

    EXPECT_TRUE(index.isAvailable(101, original));  // old dates released
    EXPECT_FALSE(index.isAvailable(101, moved));    // new dates held
}

// A one-night shift overlaps the booking being moved. Removing the old range
// before testing the new one is what makes this possible.
TEST(AvailabilityIndexTest, TryReplaceAllowsNewRangeOverlappingItsOwnBooking) {
    AvailabilityIndex index;
    index.registerRoom(101);
    DateRange original(2026y/October/5d, 2026y/October/8d);
    DateRange shifted(2026y/October/6d, 2026y/October/9d);

    ASSERT_TRUE(index.tryBook(101, original));
    EXPECT_TRUE(index.tryReplace(101, original, shifted));

    EXPECT_FALSE(index.isAvailable(101, shifted));  // new range held

    // The 5th was dropped and the 8th picked up. Checking single nights,
    // because the old and new ranges overlap each other on the 6th-7th.
    EXPECT_TRUE(index.isAvailable(101, DateRange(2026y/October/5d, 2026y/October/6d)));
    EXPECT_FALSE(index.isAvailable(101, DateRange(2026y/October/8d, 2026y/October/9d)));
}

TEST(AvailabilityIndexTest, TryReplaceFailsAndRestoresWhenNewRangeClashes) {
    AvailabilityIndex index;
    index.registerRoom(101);
    DateRange first(2026y/October/5d, 2026y/October/8d);
    DateRange second(2026y/October/12d, 2026y/October/15d);

    ASSERT_TRUE(index.tryBook(101, first));
    ASSERT_TRUE(index.tryBook(101, second));

    // Try to move `first` onto dates that clash with `second`.
    DateRange clashing(2026y/October/13d, 2026y/October/14d);
    EXPECT_FALSE(index.tryReplace(101, first, clashing));

    // Rollback: both original bookings must survive untouched.
    EXPECT_FALSE(index.isAvailable(101, first));
    EXPECT_FALSE(index.isAvailable(101, second));
}

TEST(AvailabilityIndexTest, TryReplaceLeavesOtherRoomsAlone) {
    AvailabilityIndex index;
    index.registerRoom(101);
    index.registerRoom(102);
    DateRange range(2026y/October/5d, 2026y/October/8d);

    ASSERT_TRUE(index.tryBook(101, range));
    ASSERT_TRUE(index.tryBook(102, range));

    DateRange moved(2026y/November/1d, 2026y/November/4d);
    EXPECT_TRUE(index.tryReplace(101, range, moved));

    EXPECT_TRUE(index.isAvailable(101, range));   // 101 released those dates
    EXPECT_FALSE(index.isAvailable(102, range));  // 102 untouched
}

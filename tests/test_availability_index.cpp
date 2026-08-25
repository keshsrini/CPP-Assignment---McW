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
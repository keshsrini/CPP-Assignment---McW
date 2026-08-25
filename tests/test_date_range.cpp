#include <gtest/gtest.h>
#include "hotel/DateRange.hpp"
#include "hotel/Exceptions.hpp"

using namespace std::chrono;
using hotel::DateRange;
using hotel::InvalidDateRangeException;

TEST(DateRangeTest, ValidRangeConstructsSuccessfully) {
    DateRange range(2026y/October/5d, 2026y/October/8d);
    EXPECT_EQ(range.getCheckIn(), 2026y/October/5d);
    EXPECT_EQ(range.getCheckOut(), 2026y/October/8d);
}

TEST(DateRangeTest, ThrowsWhenCheckoutBeforeCheckin) {
    EXPECT_THROW(DateRange(2026y/October/8d, 2026y/October/5d), InvalidDateRangeException);
}

TEST(DateRangeTest, ThrowsWhenCheckoutEqualsCheckin) {
    EXPECT_THROW(DateRange(2026y/October/5d, 2026y/October/5d), InvalidDateRangeException);
}

TEST(DateRangeTest, OverlappingRangesDetected) {
    DateRange a(2026y/October/5d, 2026y/October/10d);
    DateRange b(2026y/October/8d, 2026y/October/12d);
    EXPECT_TRUE(a.overlaps(b));
    EXPECT_TRUE(b.overlaps(a));
}

TEST(DateRangeTest, NonOverlappingRangesNotDetected) {
    DateRange a(2026y/October/1d, 2026y/October/5d);
    DateRange b(2026y/October/10d, 2026y/October/15d);
    EXPECT_FALSE(a.overlaps(b));
    EXPECT_FALSE(b.overlaps(a));
}

TEST(DateRangeTest, AdjacentRangesDoNotOverlap) {
    DateRange a(2026y/October/5d, 2026y/October/10d);
    DateRange b(2026y/October/10d, 2026y/October/15d);
    EXPECT_FALSE(a.overlaps(b));
    EXPECT_FALSE(b.overlaps(a));
}
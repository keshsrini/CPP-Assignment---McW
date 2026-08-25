#include <gtest/gtest.h>
#include "hotel/Guest.hpp"

using hotel::Guest;

TEST(GuestTest, ConstructorStoresValuesCorrectly) {
    Guest g("G001", "Keshav", "keshav@example.com");

    EXPECT_EQ(g.getGuestId(), "G001");
    EXPECT_EQ(g.getName(), "Keshav");
    EXPECT_EQ(g.getContactInfo(), "keshav@example.com");
}

TEST(GuestTest, HandlesEmptyContactInfoGracefully) {
    Guest g("G002", "Anita", "");

    EXPECT_EQ(g.getContactInfo(), "");
    EXPECT_EQ(g.getName(), "Anita");
}

TEST(GuestTest, DistinctGuestsAreIndependent) {
    Guest g1("G003", "Ravi", "ravi@example.com");
    Guest g2("G004", "Priya", "priya@example.com");

    EXPECT_NE(g1.getGuestId(), g2.getGuestId());
    EXPECT_NE(g1.getName(), g2.getName());
}
#include <gtest/gtest.h>
#include "hotel/RoomCategory.hpp"

using hotel::RoomCategory;

TEST(RoomCategoryTest, ConstructorStoresValuesCorrectly) {
    RoomCategory deluxe("Deluxe", 3, 4500.0, {"WiFi", "Mini Bar"});

    EXPECT_EQ(deluxe.getName(), "Deluxe");
    EXPECT_EQ(deluxe.getCapacity(), 3);
    EXPECT_DOUBLE_EQ(deluxe.getBaseRate(), 4500.0);
}

TEST(RoomCategoryTest, AmenitiesStoredCorrectly) {
    RoomCategory suite("Suite", 4, 8000.0, {"WiFi", "Jacuzzi", "Room Service"});

    const auto& amenities = suite.getAmenities();
    ASSERT_EQ(amenities.size(), 3u);
    EXPECT_EQ(amenities[0], "WiFi");
    EXPECT_EQ(amenities[1], "Jacuzzi");
    EXPECT_EQ(amenities[2], "Room Service");
}

TEST(RoomCategoryTest, EmptyAmenitiesListIsValid) {
    RoomCategory standard("Standard", 2, 2000.0, {});

    EXPECT_TRUE(standard.getAmenities().empty());
}

TEST(RoomCategoryTest, MultipleInstancesAreIndependent) {
    RoomCategory cat1("Standard", 2, 2000.0, {"WiFi"});
    RoomCategory cat2("Standard", 2, 2500.0, {"WiFi", "TV"});

    EXPECT_DOUBLE_EQ(cat1.getBaseRate(), 2000.0);
    EXPECT_DOUBLE_EQ(cat2.getBaseRate(), 2500.0);
    EXPECT_NE(cat1.getAmenities().size(), cat2.getAmenities().size());
}

TEST(RoomCategoryTest, HandlesRealisticSuiteValues) {
    RoomCategory presidential("Presidential Suite", 6, 25000.0,
                               {"WiFi", "Jacuzzi", "Butler Service", "Private Pool"});

    EXPECT_EQ(presidential.getCapacity(), 6);
    EXPECT_DOUBLE_EQ(presidential.getBaseRate(), 25000.0);
    EXPECT_EQ(presidential.getAmenities().size(), 4u);
}
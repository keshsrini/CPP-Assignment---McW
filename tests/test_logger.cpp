#include <gtest/gtest.h>
#include "hotel/Logger.hpp"

using hotel::Logger;

TEST(LoggerTest, LogDoesNotThrow) {
    Logger logger;
    EXPECT_NO_THROW(logger.log("Test event occurred"));
}

TEST(LoggerTest, LogHandlesEmptyMessage) {
    Logger logger;
    EXPECT_NO_THROW(logger.log(""));
}

TEST(LoggerTest, LogHandlesMultipleCallsInSequence) {
    Logger logger;
    EXPECT_NO_THROW({
        logger.log("Booking created");
        logger.log("Check-in completed");
        logger.log("Check-out completed");
    });
}
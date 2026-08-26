#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include "hotel/AvailabilityIndex.hpp"

using namespace std::chrono;
using hotel::AvailabilityIndex;
using hotel::DateRange;
TEST(ConcurrencyTest, ConcurrentBookingsNeverDoubleBookSameRoom) {
    AvailabilityIndex index;
    const int roomCount = 3;
    for (int room = 101; room < 101 + roomCount; ++room) {
        index.registerRoom(room);
    }

    DateRange sameRange(2026y/October/5d, 2026y/October/8d);
    const int threadsPerRoom = 5;
    std::atomic<int> successCount{0};

    std::vector<std::thread> threads;
    for (int room = 101; room < 101 + roomCount; ++room) {
        for (int i = 0; i < threadsPerRoom; ++i) {
            threads.emplace_back([&index, room, &sameRange, &successCount]() {
                if (index.tryBook(room, sameRange)) {
                    successCount++;
                }
            });
        }
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(successCount.load(), roomCount);
}
TEST(ConcurrencyTest, ConcurrentBookingsOnDifferentRoomsAllSucceed) {
    AvailabilityIndex index;
    const int roomCount = 10;
    for (int room = 201; room < 201 + roomCount; ++room) {
        index.registerRoom(room);
    }

    DateRange range(2026y/November/1d, 2026y/November/4d);
    std::atomic<int> successCount{0};

    std::vector<std::thread> threads;
    for (int room = 201; room < 201 + roomCount; ++room) {
        threads.emplace_back([&index, room, &range, &successCount]() {
            if (index.tryBook(room, range)) {
                successCount++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(successCount.load(), roomCount);
}
TEST(ConcurrencyTest, HighContentionManyThreadsOneRoomOnlyOneWins) {
    AvailabilityIndex index;
    index.registerRoom(101);

    DateRange range(2026y/December/20d, 2026y/December/25d);
    std::atomic<int> successCount{0};

    std::vector<std::thread> threads;
    for (int i = 0; i < 20; ++i) {
        threads.emplace_back([&index, &range, &successCount]() {
            if (index.tryBook(101, range)) {
                successCount++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(successCount.load(), 1);
}
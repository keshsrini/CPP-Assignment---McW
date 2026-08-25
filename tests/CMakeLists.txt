include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(hotel_tests
    test_room_category.cpp
    test_room.cpp
    test_guest.cpp
    test_date_range.cpp
    test_reservation.cpp
    test_availability_index.cpp
    test_billing.cpp
    test_logger.cpp
    test_hotel_service.cpp
    test_concurrency.cpp
)

target_link_libraries(hotel_tests PRIVATE hotel_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(hotel_tests)
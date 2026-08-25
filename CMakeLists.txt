cmake_minimum_required(VERSION 3.20) 
project(HotelReservationSystem CXX)  

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(hotel_core
    src/Room.cpp
    src/Guest.cpp
    src/RoomCategory.cpp
    src/DateRange.cpp
    src/Reservation.cpp
    src/AvailabilityIndex.cpp
    src/BillingStrategy.cpp
    src/Logger.cpp
    src/HotelService.cpp
)

target_include_directories(hotel_core PUBLIC include)
target_link_libraries(hotel_core PUBLIC Threads::Threads)


add_executable(hotel_app app/main.cpp)
target_link_libraries(hotel_app PRIVATE hotel_core)

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
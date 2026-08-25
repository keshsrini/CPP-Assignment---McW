#pragma once

#include <stdexcept>
#include <string>

namespace hotel {

class HotelException : public std::runtime_error {
public:
    explicit HotelException(const std::string& message)
        : std::runtime_error(message) {}
};

class RoomUnavailableException : public HotelException {
public:
    explicit RoomUnavailableException(const std::string& message)
        : HotelException(message) {}
};

class InvalidReservationException : public HotelException {
public:
    explicit InvalidReservationException(const std::string& message)
        : HotelException(message) {}
};

class InvalidDateRangeException : public HotelException {
public:
    explicit InvalidDateRangeException(const std::string& message)
        : HotelException(message) {}
};

class RoomNotFoundException : public HotelException {
public:
    explicit RoomNotFoundException(const std::string& message)
        : HotelException(message) {}
};

} 
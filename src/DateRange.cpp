#include "hotel/DateRange.hpp"
#include "hotel/Exceptions.hpp"

namespace hotel {

DateRange::DateRange(std::chrono::year_month_day checkIn, std::chrono::year_month_day checkOut)
    : checkIn_(checkIn), checkOut_(checkOut) {
    if (!(checkIn_ < checkOut_)) {
        throw InvalidDateRangeException("Checkout date must be strictly after checkin date");
    }
}

std::chrono::year_month_day DateRange::getCheckIn() const { return checkIn_; }
std::chrono::year_month_day DateRange::getCheckOut() const { return checkOut_; }

bool DateRange::overlaps(const DateRange& other) const {
    return checkIn_ < other.checkOut_ && other.checkIn_ < checkOut_;
}

}
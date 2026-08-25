#pragma once

#include <chrono>

namespace hotel {

class DateRange {
public:
    DateRange(std::chrono::year_month_day checkIn, std::chrono::year_month_day checkOut);

    std::chrono::year_month_day getCheckIn() const;
    std::chrono::year_month_day getCheckOut() const;

    bool overlaps(const DateRange& other) const;

private:
    std::chrono::year_month_day checkIn_;
    std::chrono::year_month_day checkOut_;
};

}
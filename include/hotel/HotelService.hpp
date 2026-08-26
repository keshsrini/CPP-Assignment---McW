#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "hotel/AvailabilityIndex.hpp"
#include "hotel/BillingStrategy.hpp"
#include "hotel/DateRange.hpp"
#include "hotel/Guest.hpp"
#include "hotel/Logger.hpp"
#include "hotel/Reservation.hpp"
#include "hotel/Room.hpp"
#include "hotel/RoomCategory.hpp"

namespace hotel {

struct CategoryOccupancy {
    std::string category;
    double occupancyRate;  
    int totalRooms = 0;
    int occupiedRooms = 0;
};

struct Report {
    std::vector<CategoryOccupancy> occupancyByCategory;
    double totalRevenue = 0.0;
    double averageStayLengthNights = 0.0;
    int bookedAwaitingCheckIn = 0;
    int currentlyCheckedIn = 0;
    int completedStays = 0;
    int cancelled = 0;
};

class HotelService {
public:
    explicit HotelService(std::unique_ptr<BillingStrategy> billingStrategy);

    void addRoom(int roomNumber, std::shared_ptr<RoomCategory> category);

    Reservation bookRoom(const std::string& categoryName, const DateRange& dates, Guest guest);

    void checkIn(int reservationId);
    double checkOut(int reservationId, double serviceCharges = 0.0);
    void cancelReservation(int reservationId);
    void modifyReservation(int reservationId, const DateRange& newDates);

    std::vector<Room*> searchRooms(const std::string& categoryName, const DateRange& dates);

    Report getReports() const;

    Room& findRoom(int roomNumber);

private:
    Reservation& findReservationUnlocked(int reservationId);

    std::vector<std::unique_ptr<Room>> rooms_;
    AvailabilityIndex availabilityIndex_;
    std::unique_ptr<BillingStrategy> billingStrategy_;
    Logger logger_;

    std::map<int, Reservation> reservations_;
    std::atomic<int> nextReservationId_{1};
    mutable std::mutex reservationsMutex_;

    double totalRevenue_ = 0.0;
    long long totalNightsCheckedOut_ = 0;
    int checkoutCount_ = 0;
};

} 
#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <string>
#include "hotel/HotelService.hpp"
#include "hotel/Exceptions.hpp"

using namespace hotel;
using namespace std::chrono;


int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "  Not a valid number, try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double readDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "  Not a valid number, try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string readLine(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    return value;
}

// Reads "DD/MM/YYYY" into a year_month_day. Loops until valid.
year_month_day readDate(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        int d, m, y;
        char slash1, slash2;
        std::istringstream iss(s);
        if ((iss >> d >> slash1 >> m >> slash2 >> y) && slash1 == '/' && slash2 == '/') {
            year_month_day ymd{year{y}, month{static_cast<unsigned>(m)}, day{static_cast<unsigned>(d)}};
            if (ymd.ok()) {
                return ymd;
            }
        }
        std::cout << "  Invalid date, expected format DD/MM/YYYY (e.g. 05/10/2026).\n";
    }
}

void printReport(const Report& report) {
    // Remember the stream's formatting so the fixed/precision settings below
    // don't leak into everything printed afterwards.
    const std::ios::fmtflags savedFlags = std::cout.flags();
    const std::streamsize savedPrecision = std::cout.precision();

    std::cout << "\n=============== REPORT ===============\n";

    std::cout << "\nOccupancy  (rooms with a guest checked in right now)\n";
    for (const auto& c : report.occupancyByCategory) {
        std::cout << "  " << std::left << std::setw(10) << c.category
                  << c.occupiedRooms << " of " << c.totalRooms << " rooms"
                  << "  (" << std::fixed << std::setprecision(1)
                  << (c.occupancyRate * 100) << "%)\n";
        std::cout.unsetf(std::ios::left);
    }

    const int totalReservations = report.bookedAwaitingCheckIn + report.currentlyCheckedIn +
                                  report.completedStays + report.cancelled;

    std::cout << "\nReservations  (" << totalReservations << " total)\n";
    std::cout << "  Booked, awaiting check-in : " << report.bookedAwaitingCheckIn << "\n";
    std::cout << "  Currently checked in      : " << report.currentlyCheckedIn << "\n";
    std::cout << "  Completed stays           : " << report.completedStays << "\n";
    std::cout << "  Cancelled                 : " << report.cancelled << "\n";

    std::cout << "\nRevenue  (billed at check-out)\n";
    std::cout << "  Total collected     : " << std::fixed << std::setprecision(2)
              << report.totalRevenue << "\n";
    std::cout << "  Average stay length : " << std::setprecision(1)
              << report.averageStayLengthNights << " nights\n";

    if (report.completedStays == 0) {
        std::cout << "\n  Note: revenue and average stay stay at 0 until a guest\n"
                  << "        checks out. Occupancy counts only checked-in guests,\n"
                  << "        so a booked room shows as free until check-in.\n";
    }

    std::cout << "\n======================================\n";

    std::cout.flags(savedFlags);
    std::cout.precision(savedPrecision);
}

void printMenu() {
    std::cout << "\n=== Hotel Reservation System ===\n"
              << "1. Book a room\n"
              << "2. Check in\n"
              << "3. Check out\n"
              << "4. Cancel reservation\n"
              << "5. Search available rooms\n"
              << "6. View reports\n"
              << "7. Exit\n";
}

int main() {
    // Set up inventory once at startup — this part stays fixed,
    // since dynamically adding rooms/categories isn't part of the assignment scope.
    auto standard = std::make_shared<RoomCategory>("Standard", 2, 2000.0,
        std::vector<std::string>{"WiFi", "TV"});
    auto deluxe = std::make_shared<RoomCategory>("Deluxe", 3, 3500.0,
        std::vector<std::string>{"WiFi", "TV", "Mini-bar"});

    HotelService hotel(std::make_unique<StandardBillingStrategy>());
    hotel.addRoom(101, standard);
    hotel.addRoom(102, standard);
    hotel.addRoom(201, deluxe);
    hotel.addRoom(202, deluxe);

    std::cout << "Hotel initialized: rooms 101, 102 (Standard), 201, 202 (Deluxe)\n";

    bool running = true;
    while (running) {
        printMenu();
        int choice = readInt("Choose an option: ");

        try {
            switch (choice) {
                case 1: {
                    std::string category = readLine("Category (Standard/Deluxe): ");
                    year_month_day checkIn = readDate("Check-in date (DD/MM/YYYY): ");
                    year_month_day checkOut = readDate("Check-out date (DD/MM/YYYY): ");
                    std::string name = readLine("Guest name: ");
                    std::string contact = readLine("Guest contact: ");

                    Guest guest("G" + std::to_string(std::rand() % 100000), name, contact);
                    DateRange dates(checkIn, checkOut);
                    Reservation r = hotel.bookRoom(category, dates, guest);

                    std::cout << ">> Booked! Reservation ID = " << r.getReservationId()
                              << ", Room = " << r.getRoomNumber() << "\n";
                    break;
                }
                case 2: {
                    int id = readInt("Reservation ID to check in: ");
                    hotel.checkIn(id);
                    std::cout << ">> Checked in reservation " << id << "\n";
                    break;
                }
                case 3: {
                    int id = readInt("Reservation ID to check out: ");
                    double extra = readDouble("Extra service charges (0 if none): ");
                    double bill = hotel.checkOut(id, extra);
                    std::cout << ">> Checked out. Total bill = " << bill << "\n";
                    break;
                }
                case 4: {
                    int id = readInt("Reservation ID to cancel: ");
                    hotel.cancelReservation(id);
                    std::cout << ">> Cancelled reservation " << id << "\n";
                    break;
                }
                case 5: {
                    std::string category = readLine("Category to search (Standard/Deluxe): ");
                    year_month_day checkIn = readDate("Check-in date (DD/MM/YYYY): ");
                    year_month_day checkOut = readDate("Check-out date (DD/MM/YYYY): ");
                    DateRange dates(checkIn, checkOut);
                    auto results = hotel.searchRooms(category, dates);

                    std::cout << ">> " << results.size() << " available room(s): ";
                    for (auto* room : results) {
                        std::cout << room->getRoomNumber() << " ";
                    }
                    std::cout << "\n";
                    break;
                }
                case 6: {
                    printReport(hotel.getReports());
                    break;
                }
                case 7: {
                    running = false;
                    std::cout << "Goodbye.\n";
                    break;
                }
                default:
                    std::cout << "Invalid option, choose 1-7.\n";
            }
        } catch (const HotelException& e) {
            // Every domain error surfaces here — nothing crashes, everything
            // is a caught, named exception type from your hierarchy.
            std::cout << "!! Error: " << e.what() << "\n";
        }
    }

    return 0;
}
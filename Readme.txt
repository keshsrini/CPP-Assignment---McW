HOTEL RESERVATION SYSTEM - C++
==============================


1. PROJECT OVERVIEW
-------------------

A hotel reservation system written in C++20. It runs in the terminal
as a menu-driven program.

The hotel starts with 4 rooms:

    Room 101, 102   Standard   (2 guests, 2000 per night)
    Room 201, 202   Deluxe     (3 guests, 3500 per night)

From the menu you can book a room for a date range, check a guest in,
check them out (which generates the bill), cancel a booking, search
for free rooms, and view occupancy and revenue reports.

A booking moves through these states:

    Booked  ->  CheckedIn  ->  CheckedOut
       |
       +------->  Cancelled

The program blocks anything else, so you cannot check in twice or
check out someone who never checked in. It shows an error instead.

Booking is thread-safe. If two threads try to book the same room for
the same dates, only one succeeds - there is no double booking.


2. FOLDER STRUCTURE
-------------------

    CMakeLists.txt      Build file. Lists the sources, sets C++20,
                        builds the library, the app and the tests.

    Readme.txt          This file.

    include/hotel/      Header files (.hpp) - what each class offers.

                        Guest.hpp              a guest
                        RoomCategory.hpp       room type + price
                        Room.hpp               an actual room
                        DateRange.hpp          check-in to check-out
                        Reservation.hpp        one booking
                        AvailabilityIndex.hpp  which dates are taken
                        BillingStrategy.hpp    bill calculation
                        logger.hpp             event logging
                        HotelService.hpp       main class, ties it all
                                               together
                        exceptions.hpp         the error types

    src/                The matching .cpp files with the actual code.
                        One .cpp per header above.

    app/main.cpp        The menu. Reads your input and calls
                        HotelService. The only file that reads the
                        keyboard.

    tests/              56 unit tests using GoogleTest, one test file
                        per class.

    build/              Compiler output. Created by CMake, not in git.
                        Safe to delete and rebuild.

    report/             For the written report.

The logic lives in src/ and is built as a library, so both the app and
the tests can use it.


3. OS / TOOLS / VERSIONS USED
-----------------------------

    Operating system    Windows 11 Home (64-bit)
    Compiler            g++ 15.2.0 (MinGW-w64)
    Build system        CMake 3.20.21032501
    Generator           MinGW Makefiles
    C++ standard        C++20
    Test framework      GoogleTest v1.15.2 (downloaded by CMake)
    Shell               Windows PowerShell

C++20 is required, not optional - the project uses std::chrono dates
and std::format, so GCC 13 or newer is needed.

Building the tests needs internet the first time, because CMake
downloads GoogleTest. After that it is cached.


4. BUILD & RUN INSTRUCTIONS
---------------------------

Open PowerShell in the folder containing CMakeLists.txt.

Configure (only needed once):

    cmake -S . -B build -G "MinGW Makefiles"

Build:

    cmake --build build

Run the tests:

    ctest --test-dir build --output-on-failure

Run the program:

    .\build\hotel_app.exe

Input formats:

    Category    type Standard or Deluxe
    Dates       DD/MM/YYYY, for example 05/10/2026
    Money       plain number, e.g. 500 or 0

To build without tests (no internet needed):

    cmake -S . -B build -G "MinGW Makefiles" -DBUILD_TESTS=OFF
    cmake --build build


5. HOW TO INTERPRET RESULTS
---------------------------

Sample booking, check-in and check-out:

    >> Booked! Reservation ID = 1, Room = 101
    >> Checked in reservation 1
    >> Checked out. Total bill = 6000

The bill of 6000 comes from a stay of 05/10/2026 to 08/10/2026:

    05/10/2026 to 08/10/2026  = 3 nights
    Standard rate             = 2000 per night
    3 x 2000 + 0 extras       = 6000

It is 3 nights, not 4 - the guest sleeps on the 5th, 6th and 7th and
leaves on the 8th.

Note down the Reservation ID when you book. You need it to check in,
check out or cancel.

Report output:

    --- REPORT ---
      Deluxe occupancy: 0.0%
      Standard occupancy: 0.0%
      Total revenue: 6000.0
      Avg stay length: 3.0 nights

    Occupancy       rooms of that type occupied right now, meaning a
                    guest is currently checked in. Goes back to 0%
                    after check-out.
    Total revenue   sum of all bills from completed check-outs. Stays
                    0 while a guest is still in the room.
    Avg stay        total nights divided by number of check-outs.

Message types:

    [timestamp] ...   a log entry confirming something happened
    >> ...            the result of what you asked for
    !! Error: ...     something was refused, and the menu comes back

Common errors and what they mean:

    No available room of category: Standard
        All Standard rooms are booked for those dates. Try Deluxe or
        different dates.

    Checkout date must be strictly after checkin date
        Check-out was on or before check-in.

    Reservation not found: 7
        No booking with that ID.

    Reservation is not in Booked state, cannot check in
        Already checked in, or the booking was cancelled.

    Reservation is not in CheckedIn state, cannot check out
        Check the guest in first.

These are not crashes - the program refuses the action and returns to
the menu.

Test output:

    100% tests passed, 0 tests failed out of 56

That is what to look for. A failing test prints [ FAILED ] with the
file, line number, expected value and actual value.

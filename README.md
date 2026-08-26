# Hotel Reservation System - C++

## 1. Project Overview

A menu-driven hotel reservation system written in C++20, running in the terminal.

The hotel starts with 10 rooms:

| Rooms | Category | Capacity | Rate |
|---|---|---|---|
| 101-105 | Standard | 2 guests | 2000 per night |
| 201-205 | Deluxe | 3 guests | 3500 per night |

You can book a room for a date range, check a guest in, check them out (which generates the bill), cancel a booking, move a booking to different dates, search for free rooms, and view occupancy and revenue reports.

A booking moves through these states, and anything else is rejected with an error:

```
Booked  ->  CheckedIn  ->  CheckedOut
   |
   +------->  Cancelled
```

Booking is thread-safe - if two threads book the same room for the same dates, only one succeeds.

## 2. Folder Structure

```
CMakeLists.txt      Build file. Sets C++20, builds the library,
                    the app and the tests.

include/hotel/      Headers. One per class: Guest, RoomCategory,
                    Room, DateRange, Reservation, AvailabilityIndex,
                    BillingStrategy, Logger, HotelService, exceptions.

src/                The matching .cpp files. One per header.

app/main.cpp        The menu. The only file that reads the keyboard.

tests/              67 GoogleTest unit tests, one file per class.

build/              Compiler output. Created by CMake, not in git.

report/             For the written report.

Readme.txt          Plain text version of this file.
```

Key classes:

- **AvailabilityIndex** - tracks which dates are taken per room and handles all locking
- **BillingStrategy** - bill calculation, swappable (standard and weekend surcharge)
- **HotelService** - the main class that coordinates everything

The logic in `src/` is built as a library, so both the app and the tests can use it.

## 3. OS / Tools / Versions Used

| | |
|---|---|
| Operating system | Windows 11 Home (64-bit) |
| Compiler | g++ 15.2.0 (MinGW-w64) |
| Build system | CMake 3.20.21032501 |
| Generator | MinGW Makefiles |
| C++ standard | C++20 (required - uses `std::chrono` dates and `std::format`) |
| Test framework | GoogleTest v1.15.2 (downloaded by CMake) |
| Shell | Windows PowerShell |

## 4. Build & Run Instructions

```powershell
cmake -S . -B build -G "MinGW Makefiles"    # configure, once
cmake --build build                          # build
ctest --test-dir build --output-on-failure   # run tests
.\build\hotel_app.exe                        # run the program
```

Run the program in a real terminal window, not an editor's built-in terminal panel. Editor panels often don't pass typed input to the program, so the menu appears but nothing responds. To launch it in its own window:

```powershell
Start-Process cmd "/k .\build\hotel_app.exe" -WorkingDirectory $PWD
```

Input formats:

| Input | Format |
|---|---|
| Category | `Standard` or `Deluxe` |
| Dates | `DD/MM/YYYY`, e.g. `05/10/2026` |
| Money | plain number, e.g. `500` or `0` |

Building the tests needs internet the first time, as CMake downloads GoogleTest. To skip them, add `-DBUILD_TESTS=OFF` when configuring.

## 5. How to Interpret Results

A booking, check-in and check-out:

```
>> Booked! Reservation ID = 1, Room = 101
>> Checked in reservation 1
>> Checked out. Total bill = 6000
```

The bill for 05/10/2026 to 08/10/2026 is 3 nights x 2000 = 6000. It is 3 nights, not 4 - the guest sleeps on the 5th, 6th and 7th and leaves on the 8th.

Note down the Reservation ID. You need it to check in, check out, cancel or modify.

Report output (option 6):

```
=== REPORT ===

Occupancy  (rooms with a guest checked in right now)
  Deluxe    0 of 5 rooms  (0.0%)
  Standard  0 of 5 rooms  (0.0%)

Reservations  (1 total)
  Booked, awaiting check-in : 0
  Currently checked in      : 0
  Completed stays           : 1
  Cancelled                 : 0

Revenue  (billed at check-out)
  Total collected     : 6000.00
  Average stay length : 3.0 nights
```

- **Occupancy** counts only guests checked in right now, so a booked room still shows as free and it returns to 0% after check-out
- **Reservations** is where a new booking appears immediately
- **Revenue** and **average stay** only count completed check-outs, so they stay at 0 until a guest checks out

Output prefixes:

| Prefix | Meaning |
|---|---|
| `[timestamp]` | a log entry |
| `>>` | the result of your action |
| `!! Error:` | the action was refused, and the menu returns |

Errors are not crashes. Invalid dates, unknown reservation IDs, a full category, or an action in the wrong order are all reported and the program continues.

Test output to look for:

```
100% tests passed, 0 tests failed out of 67
```

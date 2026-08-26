# Hotel Reservation System - C++

## 1. Project Overview

A menu-driven hotel reservation system written in C++20, running in the terminal.

The hotel starts with 10 rooms:
101 - 105    Standard    2 guests    2000/night

201 - 205    Deluxe      3 guests    3500/night


You can book a room for a date range, check a guest in, check them out (which generates the bill), cancel a booking, move a booking to different dates, search for free rooms, and view occupancy and revenue reports.

A booking moves through these states, and anything else is rejected with an error:

Booked->Checked In->Checked Out
Booked->Cancelled
Booked->Modified->Checked In->CheckedOut



Booking is thread-safe - if two threads book the same room for the same dates, only one succeeds.

## 2. Folder Structure

```
CMakeLists.txt      Build file. Sets C++20, builds the library,
                    the app and the tests.

include/hotel/      Headers. One per class: Guest, RoomCategory,
                    Room, DateRange, Reservation, AvailabilityIndex,
                    BillingStrategy, Logger, HotelService, exceptions.

src/                The matching .cpp files. One per header.

app/main.cpp        The main app

tests/              67 GoogleTest unit tests, one file per class.

build/              Compiler output. Created by CMake, not in git as mentioned

report/             For the written report.

Readme.txt          Plain text version of this file.
```

Key classes:

- **AvailabilityIndex** - tracks which dates are taken per room and handles all locking
- **BillingStrategy** - bill calculation, swappable (standard and weekend surcharge)
- **HotelService** - the main class that coordinates everything

The logic in `src/` is built as a library, so both the app and the tests can use it.

## 3. OS / Tools / Versions Used

####1.OS - Win 11
####2.Compiler - g++ 15.2.0
####3.Build System - CMake 3.20
####4.Test Framework - Google Test v1.15.2
####5.Shell - Windows Powershell


## 4. Build & Run Instructions

```powershell
cmake -S . -B build -G "MinGW Makefiles"     # configure, once
cmake --build build                          # build
ctest --test-dir build                       # run tests
.\build\hotel_app.exe                        # run the program
```

You can also run using this command

```powershell
Start-Process .\build\hotel_app.exe
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

The bill for 05/10/2026 to 08/10/2026 is 3 nights x 2000 = 6000.

Note down the Reservation ID. You need it to check in, check out, cancel or modify using that only

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

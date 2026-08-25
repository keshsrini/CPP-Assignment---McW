#include "hotel/Logger.hpp"
#include <iostream>
#include <chrono>
#include <format>

namespace hotel {

void Logger::log(const std::string& event) const {
    auto now = std::chrono::system_clock::now();
    std::cout << std::format("[{:%d/%m/%Y %H:%M:%S}] {}\n",
                              std::chrono::floor<std::chrono::seconds>(now), event);
}

} 
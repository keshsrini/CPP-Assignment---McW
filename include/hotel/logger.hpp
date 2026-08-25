#pragma once

#include <string>

namespace hotel {

class Logger {
public:
    void log(const std::string& event) const;
};

} 
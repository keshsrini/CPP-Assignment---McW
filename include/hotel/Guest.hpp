#pragma once

#include <string>

namespace hotel {

class Guest {
public:
    Guest(std::string guestId, std::string name, std::string contactInfo);

    const std::string& getGuestId() const;
    const std::string& getName() const;
    const std::string& getContactInfo() const;

private:
    std::string guestId_;
    std::string name_;
    std::string contactInfo_;
};

}
#include "hotel/Guest.hpp"

namespace hotel {

Guest::Guest(std::string guestId, std::string name, std::string contactInfo)
    : guestId_(std::move(guestId)),
      name_(std::move(name)),
      contactInfo_(std::move(contactInfo)) {}

const std::string& Guest::getGuestId() const { return guestId_; }
const std::string& Guest::getName() const { return name_; }
const std::string& Guest::getContactInfo() const { return contactInfo_; }

} 
#ifndef __RR32CAN__ENGINE_H__
#define __RR32CAN__ENGINE_H__

#include <cstring>

#include "RR32Can/constants.h"

namespace RR32Can {

/*
 * \brief Class Engine
 */
class Engine {
 public:
  enum class AvailabilityStatus { EMPTY = 0, NAME_KNOWN, FULL_DETAILS };

  Engine() { reset(); }

  void reset() {
    availability = AvailabilityStatus::EMPTY;
    number = 0;
    memset(this->name, '\0', kEngineNameLength + 1);
  }

  void setName(uint8_t number, const char* name) {
    reset();
    availability = AvailabilityStatus::NAME_KNOWN;
    number = number;
    strncpy(this->name, name, kEngineNameLength);
  }

  uint8_t getNumber() const { return number; }
  const char* getName() const { return name; }

  AvailabilityStatus getAvailability() const { return availability; }
  bool isNameKnown() const { return availability != AvailabilityStatus::EMPTY; }
  bool isFullDetailsKnown() const {
    return availability == AvailabilityStatus::FULL_DETAILS;
  }
  bool isNameOnlyKnown() const {
    return availability == AvailabilityStatus::NAME_KNOWN;
  }

 protected:
  AvailabilityStatus availability;
  uint8_t number;
  char name[kEngineNameLength + 1];
};

}  // namespace RR32Can

#endif  // __RR32CAN__ENGINE_H__

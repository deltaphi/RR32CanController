#ifndef __RR32CAN__ENGINE_H__
#define __RR32CAN__ENGINE_H__

#include <cstring>

#include "RR32Can/Constants.h"

namespace RR32Can {

/*
 * \brief Class Engine
 */
class EngineShortInfo {
 public:
  enum class AvailabilityStatus { EMPTY = 0, NAME_KNOWN, FULL_DETAILS };

  EngineShortInfo() : availability(AvailabilityStatus::EMPTY) { eraseName(); }

  virtual void reset() {
    availability = AvailabilityStatus::EMPTY;
    eraseName();
  }

  void setName(const char* name) {
    reset();
    availability = AvailabilityStatus::NAME_KNOWN;
    strncpy(this->name, name, kEngineNameLength);
  }

  /**
   * \brief Sets the name only when it is different from the current name.
   */
  void setNameConditional(const char* name) {
    if (strncmp(name, this->name, sizeof(this->name)) != 0) {
      setName(name);
    }
  }

  const char* getName() const { return name; }

  AvailabilityStatus getAvailability() const { return availability; }
  bool isNameKnown() const { return availability != AvailabilityStatus::EMPTY; }

  bool isNameOnlyKnown() const {
    return availability == AvailabilityStatus::NAME_KNOWN;
  }

  bool isFree() const { return availability == AvailabilityStatus::EMPTY; }

  void print() const;

 protected:
  AvailabilityStatus availability;
  char name[kEngineNameLength + 1];

  void eraseName() { memset(this->name, '\0', kEngineNameLength + 1); }
};

class Engine : public EngineShortInfo {
 public:
  using EngineShortInfo::EngineShortInfo;

  void reset() {
    // Remove all data of this class
  }

  bool isFullDetailsKnown() const {
    return availability == AvailabilityStatus::FULL_DETAILS;
  }

 protected:
};

}  // namespace RR32Can

#endif  // __RR32CAN__ENGINE_H__

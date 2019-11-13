#ifndef __RR32CAN__ENGINE_H__
#define __RR32CAN__ENGINE_H__

#include <array>
#include <cstring>

#include "RR32Can/Constants.h"
#include "RR32Can/Types.h"

namespace RR32Can {

/*
 * \brief Locomotive with a name only.
 */
class LocomotiveShortInfo {
 public:
  enum class AvailabilityStatus { EMPTY = 0, NAME_KNOWN, FULL_DETAILS };

  LocomotiveShortInfo() : availability(AvailabilityStatus::EMPTY) {
    eraseName();
  }

  virtual ~LocomotiveShortInfo() = default;

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
   *
   * \return True if the engine was changed, false otherwise.
   */
  bool setNameConditional(const char* name) {
    if (availability == AvailabilityStatus::EMPTY ||
        strncmp(name, this->name, kEngineNameLength) != 0) {
      setName(name);
      return true;
    } else {
      return false;
    }
  }

  const char* getName() const { return name; }

  AvailabilityStatus getAvailability() const { return availability; }
  bool isNameKnown() const { return availability != AvailabilityStatus::EMPTY; }

  bool isNameOnlyKnown() const {
    return availability == AvailabilityStatus::NAME_KNOWN;
  }

  bool isFree() const { return availability == AvailabilityStatus::EMPTY; }

  virtual void print() const;

 protected:
  AvailabilityStatus availability;
  char name[kEngineNameLength + 1];

  void eraseName() { memset(this->name, '\0', kEngineNameLength + 1); }

  friend class LocoConsumer;
};

/**
 * \brief Locomotive with full data.
 */
class Locomotive : public LocomotiveShortInfo {
 public:
  using LocomotiveShortInfo::LocomotiveShortInfo;

  using Uid_t = RR32Can::Uid_t;
  // Velocities have a range of 0..1000 (..1023).
  using Velocity_t = RR32Can::Velocity_t;
  using Address_t = RR32Can::EngineAddress_t;
  using FunctionBits_t = RR32Can::FunctionBits_t;
  using ProtocolName_t = std::array<char, 8>;

  void reset() {
    // Remove all data of this class
    LocomotiveShortInfo::reset();
    uid = 0;
    velocity = 0;
    direction = RR32Can::EngineDirection::UNKNOWN;
    address = 0;
    functionBits = 0;
    memset(protocol.data(), 0, protocol.size());
  }

  bool isFullDetailsKnown() const {
    return availability == AvailabilityStatus::FULL_DETAILS;
  }

  void setUid(Uid_t uid) { this->uid = uid; }
  Uid_t getUid() const { return uid; }

  /// Get the UID with only the two low bytes set.
  Uid_t getUidMasked() const { return uid & 0xFFFF; }

  void setVelocity(Velocity_t velocity) { this->velocity = velocity; }
  Velocity_t getVelocity() const { return velocity; }

  void setDirection(RR32Can::EngineDirection direction) {
    this->direction = direction;
  }
  RR32Can::EngineDirection getDirection() const { return direction; }
  void changeDirection() {
    if (direction == EngineDirection::FORWARD) {
      direction = EngineDirection::REVERSE;
    } else if (direction == EngineDirection::REVERSE) {
      direction = EngineDirection::FORWARD;
    }
  }

  void setAddress(Address_t address) { this->address = address; }
  Address_t getAddress() const { return address; }

  FunctionBits_t getFunctionBits() const { return functionBits; }

  void setFunction(uint8_t function, bool onOff) {
    FunctionBits_t mask = 1;
    mask <<= function;
    if (onOff != 0) {
      functionBits |= mask;
    } else {
      functionBits &= ~mask;
    }
  }

  bool getFunction(uint8_t function) const {
    FunctionBits_t mask = 1;
    mask <<= function;
    if ((functionBits & mask) != 0) {
      return true;
    } else {
      return false;
    }
  }

  void print() const override;

  void setProtocolString(const char* protocolString) {
    strncpy(protocol.data(), protocolString, protocol.size());
  }
  const char* getProtocolString() const { return protocol.data(); }

 protected:
  Uid_t uid;
  Velocity_t velocity;
  RR32Can::EngineDirection direction;
  Address_t address;
  FunctionBits_t functionBits;
  ProtocolName_t protocol;
};

}  // namespace RR32Can

#endif  // __RR32CAN__ENGINE_H__

#ifndef __RR32CAN__TYPES_H__
#define __RR32CAN__TYPES_H__

#include <cstdint>

namespace RR32Can {

/// Accessory direction
enum class TurnoutDirection : uint8_t { RED = 0, GREEN = 1 };

template <typename IntegralType>
constexpr IntegralType TurnoutDirectionToIntegral(TurnoutDirection dir) {
  return static_cast<IntegralType>(dir);
}

template <typename IntegralType>
constexpr TurnoutDirection TurnoutDirectionFromIntegral(IntegralType dir) {
  return (dir == 0) ? TurnoutDirection::RED : TurnoutDirection::GREEN;
}

/**
 * \brief Base class for all turnout addresses.
 *
 * Used to implement a C++ strong type for Turnout Addresses.
 *
 * Human addresses are 1-based, whereas machine addresses are 0-based.
 */
class TurnoutAddressBase {
 public:
  using value_type = uint16_t;
  constexpr value_type value() const { return addr; }

  TurnoutAddressBase() = default;
  constexpr TurnoutAddressBase(value_type addr) : addr(addr) {}

 private:
  value_type addr;
};

class MachineTurnoutAddress;

/**
 * \brief The Human-readable version of the Turnout Address.
 */
class HumanTurnoutAddress : public TurnoutAddressBase {
 public:
  using TurnoutAddressBase::TurnoutAddressBase;
  HumanTurnoutAddress(const MachineTurnoutAddress& other);
};

/**
 * \brief The Machine-readable version of the Turnout Address.
 */
class MachineTurnoutAddress : public TurnoutAddressBase {
 public:
  MachineTurnoutAddress() = default;
  using TurnoutAddressBase::TurnoutAddressBase;
  MachineTurnoutAddress(const HumanTurnoutAddress& other);
};

/// Engine Direction
enum class EngineDirection {
  UNCHANGED = 0,
  FORWARD = 1,
  REVERSE = 2,
  CHANGE_DIRECTION = 3,
  UNKNOWN = 4
};

EngineDirection switchDirection(EngineDirection);

/// State of the overall System
enum class SystemState { UNKNOWN = 0, ON, OFF };

enum class ConfigDataStreamType {
  NONE = 0,
  LOKOMOTIVE,    /* Database of all engines */
  MAGNETARTIKEL, /* Database of all configured accessories */
  FAHRSTRASSE,   /* Database of all configured multi-accessory paths */
  GLEISBILD,     /* Track layout */
  LOKINFO, /* Information about a single engine sufficient for a slave device */
  LOKNAMEN, /* Information about names of engines known to the master controller
             */
  MAGINFO,
  LOKDB,
  LDBVER
};

/// Data type to carry engine velocities. Range: 1..1000
using Uid_t = uint32_t;
using Velocity_t = uint16_t;
using EngineAddress_t = uint32_t;
using FunctionBits_t = uint16_t;

}  // namespace RR32Can

#endif  // __RR32CAN__TYPES_H__

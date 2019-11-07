#ifndef __RR32CAN__TYPES_H__
#define __RR32CAN__TYPES_H__

#include <cstdint>

namespace RR32Can {

/// Accessory direction
enum class TurnoutDirection : uint8_t { RED = 0, GREEN = 1 };

/// Engine Direction
enum class EngineDirection { FORWARD = 0, REVERSE = 1, UNKNOWN = 2 };

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
using Velocity_t = uint16_t;

}  // namespace RR32Can

#endif  // __RR32CAN__TYPES_H__

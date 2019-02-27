#ifndef __RR32CAN__TYPES_H__
#define __RR32CAN__TYPES_H__

#include <cstdint>

namespace RR32Can {

/// Accessory direction
enum class TurnoutDirection : uint8_t { RED = 0, GREEN = 1 };

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

}  // namespace RR32Can

#endif  // __RR32CAN__TYPES_H__

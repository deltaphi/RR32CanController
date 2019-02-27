#ifndef __RR32CAN__TYPES_H__
#define __RR32CAN__TYPES_H__

#include <cstdint>

namespace RR32Can {

/// Accessory direction
enum class TurnoutDirection : uint8_t { RED = 0, GREEN = 1 };

/// State of the overall System
enum class SystemState { UNKNOWN = 0, ON, OFF };

}  // namespace RR32Can

#endif  // __RR32CAN__TYPES_H__

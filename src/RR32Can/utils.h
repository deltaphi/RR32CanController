#ifndef __RR32CAN__UTILS_H__
#define __RR32CAN__UTILS_H__

#include <cstdint>

namespace RR32Can {

uint16_t computeSenderHash(uint32_t uid);

}  // namespace RR32Can

#endif  // __RR32CAN__UTILS_H__

#include "RR32Can/util/utils.h"

namespace RR32Can {

uint16_t computeSenderHash(uint32_t uid) {
  uid = ((uid >> 3) & 0xFFFFFF00) | (uid & 0x7F);
  uint16_t lowBytes = uid;
  uint16_t highBytes = (uid >> 16);
  return lowBytes ^ highBytes;
}

}  // namespace RR32Can

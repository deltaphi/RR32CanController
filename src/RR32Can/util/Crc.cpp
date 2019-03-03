#include "RR32Can/util/Crc.h"

namespace RR32Can {

void Crc::updateCrc(element_type input) {
  accumulator ^= (input << 8);
  for (int i = 0; i < 8; ++i) {
    if ((accumulator & 0x8000) != 0) {
      accumulator <<= 1;
      accumulator ^= polynomial;
    } else {
      accumulator <<= 1;
    }
  }
}

} /* namespace RR32Can */

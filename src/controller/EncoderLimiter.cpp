#include "controller/EncoderLimiter.h"

namespace controller {

EncoderLimiter::EncoderValue_t EncoderLimiter::limitedValue(
    EncoderValue_t value) {
  if (value < minimum) {
    return minimum;
  } else if (value > maximum) {
    return maximum;
  } else {
    return value;
  }
}

}  // namespace controller

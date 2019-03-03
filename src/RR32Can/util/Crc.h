#ifndef SRC_RR32CAN_UTIL_CRC_H_
#define SRC_RR32CAN_UTIL_CRC_H_

#include <cstdint>

namespace RR32Can {

class Crc {
 public:
  using size_type = uint8_t;
  using value_type = uint16_t;
  using element_type = uint8_t;

  constexpr static const value_type polynomial = 0x1021;
  constexpr static const value_type initialValue = 0xFFFF;

  void reset() {
    accumulator = initialValue;
    reference = initialValue;
  }
  void updateCrc(element_type input);
  value_type getCrc() const { return accumulator; }

  void loadReference(value_type referenceCrc) { reference = referenceCrc; }
  value_type getReference() const { return reference; }

  bool isCrcValid() const { return accumulator == reference; }

 private:
  value_type accumulator;
  value_type reference;
};

} /* namespace RR32Can */

#endif  // SRC_RR32CAN_UTIL_CRC_H_

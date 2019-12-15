
#include "model/TurnoutTypes.h"

namespace model {

void print(const TurnoutLookupResult& result) {
  Serial.print("Mode: ");
  Serial.print(static_cast<int>(result.mode), DEC);
  Serial.print(", Address: ");
  Serial.print(result.address, DEC);
  Serial.println();
}

void print(const TurnoutAction& result) {
  printf(
      "Address: %i, Direction: %i (%s)\n", result.address,
      static_cast<uint8_t>(result.direction),
      (result.direction == RR32Can::TurnoutDirection::RED ? "RED" : "GREEN"));
}

} /* namespace model */

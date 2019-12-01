
#include "model/TurnoutTypes.h"

namespace model {

void print(const TurnoutLookupResult& result) {
  Serial.print("Mode: ");
  Serial.print(static_cast<int>(result.mode), DEC);
  Serial.print(", Address: ");
  Serial.print(result.address, DEC);
  Serial.println();
}

} /* namespace model */

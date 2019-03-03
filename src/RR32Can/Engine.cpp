#include "RR32Can/Engine.h"

#include <Arduino.h>

namespace RR32Can {

void EngineShortInfo::print() const {
  Serial.print("  EngineShortInfo: ");

  if (availability == AvailabilityStatus::EMPTY) {
    Serial.println("Empty.");
  } else {
    Serial.print("'");
    Serial.print(name);
    Serial.println("'");
  }
}

}  // namespace RR32Can

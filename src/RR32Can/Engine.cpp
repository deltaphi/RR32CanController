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

void Engine::print() const {
  EngineShortInfo::print();
  if (availability == AvailabilityStatus::FULL_DETAILS) {
    Serial.print("Velocity: ");
    Serial.println(velocity, DEC);
    Serial.print("Direction: ");
    Serial.println(static_cast<uint8_t>(direction), DEC);
  }
}

}  // namespace RR32Can

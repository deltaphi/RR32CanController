#include <Arduino.h>
#include <RR32Can/Locomotive.h>

namespace RR32Can {

void LocomotiveShortInfo::print() const {
  Serial.print("  EngineShortInfo: ");

  if (availability == AvailabilityStatus::EMPTY) {
    Serial.println("Empty.");
  } else {
    Serial.print("'");
    Serial.print(name);
    Serial.println("'");
  }
}

void Locomotive::print() const {
  LocomotiveShortInfo::print();
  if (availability == AvailabilityStatus::FULL_DETAILS) {
    printf("UID: %#10x Proto: %s Addr: %i V: %i Dir: %i, F: %i\n", uid,
           protocol.data(), address, velocity, static_cast<uint8_t>(direction),
           functionBits);
  }
}

}  // namespace RR32Can

#include "RR32Can/EngineControl.h"

#include <Arduino.h>

#include "config.h"

namespace RR32Can {

const char* EngineControl::kFilenameEngine = "lokinfo";
const char* EngineControl::kFilenameEngineResult = "lokomotive";
const char* EngineControl::kEngineKeyUid = "uid";
const char* EngineControl::kEngineKeyVelocity = "velocity";
const char* EngineControl::kEngineKeyDirection = "direction";

const char* EngineControl::kNoEngineSelected = "- No Engine -";

void EngineControl::consumeConfigData(BufferManager& section,
                                      BufferManager& key,
                                      BufferManager& value) {
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
  Serial.print("EngineControl::consumeConfigData(");
  Serial.print(section.data());
  Serial.print(", ");
  Serial.print(key.data());
  Serial.print(", ");
  Serial.print(value.data());
  Serial.println(");");
#endif

  if (strncmp(kFilenameEngineResult, section.data(), section.length()) == 0) {
    /* It is a lokomotive */
    if (strncmp(kEngineKeyUid, section.data(), section.length()) == 0) {
      // UID
      currentEngine.setUid(strtoul(value.data(), NULL, 16));
      // Serial.print("Setting UID: ");
      // Serial.println(currentEngine.getUid());
    } else if (strncmp(kEngineKeyVelocity, section.data(), section.length()) ==
               0) {
      // Velocity
      currentEngine.setVelocity(atoi(value.data()));
      // Serial.print("Setting Velocity: ");
      // Serial.println(currentEngine.getVelocity());
    } else if (strncmp(kEngineKeyDirection, section.data(), section.length()) ==
               0) {
      // Direction
      currentEngine.setDirection(static_cast<RR32Can::EngineDirection>(
          atoi(value.data())));  // TODO: Unknown if this is correct.
    }                            /* else: Unused data item. */
  }
}

void EngineControl::setStreamComplete() {
  currentEngine.availability = Engine::AvailabilityStatus::FULL_DETAILS;
  // TBD: Update Display
  Serial.print("Downloaded Engine: ");
  currentEngine.print();
  Serial.println();
}

void EngineControl::setStreamAborted(){};

}  // namespace RR32Can

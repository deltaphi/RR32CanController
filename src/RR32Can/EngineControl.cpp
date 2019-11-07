#include "RR32Can/EngineControl.h"

#include <Arduino.h>

#include "config.h"

namespace RR32Can {

const char* EngineControl::kFilenameEngine = "lokinfo";
const char* EngineControl::kFilenameEngineResult = "lokomotive";
const char* EngineControl::kEngineKeyUid = "uid";
const char* EngineControl::kEngineKeyVelocity = "velocity";
const char* EngineControl::kEngineKeyDirection = "richtung";
const char* EngineControl::kEngineKeyProtocol = "typ";
const char* EngineControl::kEngineKeyAddress = "adresse";

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
    if (strncmp(kEngineKeyUid, key.data(), key.length()) == 0) {
      // UID
      currentEngine.setUid(strtoul(value.data(), NULL, 16));
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
      printf("Setting UID\n");
#endif

    } else if (strncmp(kEngineKeyVelocity, key.data(), key.length()) == 0) {
      // Velocity
      currentEngine.setVelocity(strtoul(value.data(), NULL, 10));
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
      printf("Setting Velocity\n");
#endif
    } else if (strncmp(kEngineKeyDirection, key.data(), key.length()) == 0) {
      // Direction
      currentEngine.setDirection(static_cast<RR32Can::EngineDirection>(strtoul(
          value.data(), NULL, 10)));  // TODO: Unknown if this is correct.
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
      printf("Setting direction\n");
#endif
    } else if (strncmp(kEngineKeyProtocol, key.data(), key.length()) == 0) {
      currentEngine.setProtocolString(value.data());
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
      printf("Setting protocol\n");
#endif
    } else if (strncmp(kEngineKeyAddress, key.data(), key.length()) == 0) {
      currentEngine.setAddress(strtoul(value.data(), NULL, 16));
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
      printf("Setting Address\n");
#endif
    } /* else: Unused data item. */
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

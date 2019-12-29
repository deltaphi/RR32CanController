#include <Arduino.h>
#include <RR32Can/LocoConsumer.h>
#include "config.h"

#include "RR32Can/RR32Can.h"

namespace RR32Can {

const char* LocoConsumer::kFilenameEngine = "lokinfo";
const char* LocoConsumer::kFilenameEngineResult = "lokomotive";
const char* LocoConsumer::kEngineKeyUid = "uid";
const char* LocoConsumer::kEngineKeyVelocity = "velocity";
const char* LocoConsumer::kEngineKeyDirection = "richtung";
const char* LocoConsumer::kEngineKeyProtocol = "typ";
const char* LocoConsumer::kEngineKeyAddress = "adresse";

const char* LocoConsumer::kNoEngineSelected = "- No Engine -";

void LocoConsumer::consumeConfigData(BufferManager& section, BufferManager& key,
                                     BufferManager& value) {
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
  printf("EngineControl::consumeConfigData(\"%s\", \"%s\", \"%s\")\n", section.data(), key.data(), value.data());
#endif

  if (currentEngine == nullptr) {
    return;
  }

  if (strncmp(kFilenameEngineResult, section.data(), section.length()) == 0) {
    /* It is a lokomotive */
    if (strncmp(kEngineKeyUid, key.data(), key.length()) == 0) {
      // UID
      currentEngine->setUid(strtoul(value.data(), NULL, 16));
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
      printf("Setting UID\n");
#endif

    } else if (strncmp(kEngineKeyProtocol, key.data(), key.length()) == 0) {
      currentEngine->setProtocolString(value.data());
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
      printf("Setting protocol\n");
#endif
    } else if (strncmp(kEngineKeyAddress, key.data(), key.length()) == 0) {
      currentEngine->setAddress(strtoul(value.data(), NULL, 16));
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
      printf("Setting Address\n");
#endif
    } /* else: Unused data item. */
  }
}

void LocoConsumer::setStreamComplete() {
  if (currentEngine == nullptr) {
    return;
  }

  currentEngine->availability = Locomotive::AvailabilityStatus::FULL_DETAILS;

  printf("Downloaded Engine: ");
  currentEngine->print();
  printf("\n");
}

void LocoConsumer::setStreamAborted(){};

}  // namespace RR32Can

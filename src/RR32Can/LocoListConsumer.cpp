#include <Arduino.h>
#include <RR32Can/LocoListConsumer.h>
#include "config.h"

namespace RR32Can {

const char* LocoListConsumer::kFilenameEngineNames = "loknamen";
const char* LocoListConsumer::kSectionNumEngines = "numloks";
const char* LocoListConsumer::kGenericValue = "wert";

void LocoListConsumer::consumeConfigData(BufferManager& section,
                                         BufferManager& key,
                                         BufferManager& value) {
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
  printf("EngineBrowser::consumeConfigData(\"%s\", \"%s\", \"%s\")\n", section.data(), key.data(), value.data());
#endif

  if (section.strncmp(kFilenameEngineNames)) {
    // Engine data
    LocomotiveShortInfo* freeEngine = findFirstFreeEntry();
    if (freeEngine == nullptr) {
      Serial.println("No free Engine buffer found.");
      return;
    } else {
      Serial.print("Storing Engine ");
      Serial.println(value.data());
    }
    freeEngine->setName(value.data());  // TODO: Number is unused

  } else if (section.strncmp(kSectionNumEngines)) {
    // Overall engine count
    if (key.strncmp(kGenericValue)) {
      numEnginesKnownByMaster = value.asUint8();
    }  // else: unknown key

  } else {
    Serial.print("Received config data for uninteresting section '");
    Serial.print(section.data());
    Serial.println("'.");
  }
}

void LocoListConsumer::clearTable() {
  for (LocomotiveShortInfo& info : engineInfo) {
    info.reset();
  }
}

void LocoListConsumer::reset() {
  clearTable();
  numEnginesKnownByMaster = 0;
  streamComplete = false;
  streamOffset = 0;
}

void LocoListConsumer::printAll() const {
  Serial.print("EngineBrowser: ");
  Serial.print(numEnginesKnownByMaster);
  Serial.println(" Total Engines.");
  for (const LocomotiveShortInfo& info : engineInfo) {
    info.print();
  }
}

}  // namespace RR32Can

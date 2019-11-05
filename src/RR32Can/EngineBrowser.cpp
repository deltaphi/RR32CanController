#include "RR32Can/EngineBrowser.h"

#include <Arduino.h>

#include "config.h"

namespace RR32Can {

const char* EngineBrowser::kFilenameEngineNames = "loknamen";
const char* EngineBrowser::kSectionNumEngines = "numloks";
const char* EngineBrowser::kGenericValue = "wert";

void EngineBrowser::consumeConfigData(BufferManager& section,
                                      BufferManager& key,
                                      BufferManager& value) {
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
  Serial.print("EngineBrowser::consumeConfigData(");
  Serial.print(section.data());
  Serial.print(", ");
  Serial.print(key.data());
  Serial.print(", ");
  Serial.print(value.data());
  Serial.println(")");
#endif

  if (section.strncmp(kFilenameEngineNames)) {
    // Engine data
    EngineShortInfo* freeEngine = findFirstFreeEntry();
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

void EngineBrowser::clearTable() {
  for (EngineShortInfo& info : engineInfo) {
    info.reset();
  }
}

void EngineBrowser::reset() {
  clearTable();
  numEnginesKnownByMaster = 0;
  streamComplete = false;
  cursor = 0;
}

void EngineBrowser::printAll() const {
  Serial.print("EngineBrowser: ");
  Serial.print(numEnginesKnownByMaster);
  Serial.println(" Total Engines.");
  for (const EngineShortInfo& info : engineInfo) {
    info.print();
  }
}

}  // namespace RR32Can

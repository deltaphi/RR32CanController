#include "hal/storage/TurnoutMap.h"

#include "SPIFFS.h"

namespace hal {
namespace storage {

const char* TurnoutMap::kTurnoutMapSettingsFile = "/turnoutmap.prefs";

bool TurnoutMap::load(application::model::TurnoutMap& turnoutMap) {
  File f = SPIFFS.open(kTurnoutMapSettingsFile, "r");
  if (!f) {
    printf("Opening '%s' for reading failed.\n", kTurnoutMapSettingsFile);
    return false;
  } else {
    size_t readBytes = f.read(reinterpret_cast<uint8_t*>(turnoutMap.getDb()),
                              sizeof(application::model::TurnoutMap::TurnoutMap_t));
    f.close();
    printf("%s: Loaded %i bytes.\n", kTurnoutMapSettingsFile, readBytes);
    return true;
  }
}

void TurnoutMap::store(const application::model::TurnoutMap& turnoutMap) {
  File f = SPIFFS.open(kTurnoutMapSettingsFile, "w");

  if (!f) {
    printf("Opening '%s' for writing failed.\n", kTurnoutMapSettingsFile);
  } else {
    size_t writtenBytes = f.write(reinterpret_cast<const uint8_t*>(turnoutMap.getDb()),
                                  sizeof(application::model::TurnoutMap::TurnoutMap_t));
    printf("%s: Wrote %i/%i bytes.\n", kTurnoutMapSettingsFile, writtenBytes,
           sizeof(application::model::TurnoutMap::TurnoutMap_t));

    f.close();
  }
}

}  // namespace storage
}  // namespace hal

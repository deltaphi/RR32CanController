#include "model/Settings.h"

#include "SPIFFS.h"

namespace model {

const char* Settings::kSettingsFilename = "/settings.prefs";

void Settings::begin() {
  // Setup defaults

  // Load configuration
  load();
}

bool Settings::load() {
  File f = SPIFFS.open(kSettingsFilename);
  if (!f) {
    printf("Opening '%s' for reading failed.\n", kSettingsFilename);
    return false;
  } else {
    size_t readBytes = f.read(reinterpret_cast<uint8_t*>(&data), sizeof(Data));
    f.close();
    printf("Loaded %i bytes.\n", readBytes);
    return true;
  }
}

void Settings::store() {
  File f = SPIFFS.open(kSettingsFilename);

  if (!f) {
    printf("Opening '%s' for writing failed.\n", kSettingsFilename);
  } else {
    f.write(reinterpret_cast<uint8_t*>(&data), sizeof(Data));

    f.close();
  }
}

}  // namespace model

#include "model/Settings.h"

#include "SPIFFS.h"

namespace model {

const char* Settings::kSettingsFilename = "/settings.prefs";

void Settings::begin() {
  // Setup defaults

  // Load configuration
  load();
}

size_t Settings::loadData(Data* data) {
  if (data == nullptr) {
    return 0;
  }

  File f = SPIFFS.open(kSettingsFilename);
  if (!f) {
    printf("Opening '%s' for reading failed.\n", kSettingsFilename);
    return 0;
  } else {
    size_t readBytes = f.read(reinterpret_cast<uint8_t*>(data), sizeof(Data));
    f.close();
    printf("%s: Loaded %i bytes.\n", kSettingsFilename, readBytes);
    return readBytes;
  }
}

bool Settings::load() { return loadData(&data) != 0; }

void Settings::store() {
  Data tmpData;
  size_t readBytes = loadData(&tmpData);

  if (readBytes != sizeof(Data) || memcmp(&data, &tmpData, sizeof(Data) != 0)) {
    // We need to store
    File f = SPIFFS.open(kSettingsFilename, "w");

    if (!f) {
      printf("Opening '%s' for writing failed.\n", kSettingsFilename);
    } else {
      size_t writtenBytes =
          f.write(reinterpret_cast<uint8_t*>(&data), sizeof(Data));
      printf("%s: Wrote %i/%i bytes.\n", kSettingsFilename, writtenBytes,
             sizeof(Data));

      f.close();
    }
  } else {
    printf("No settings change.\n");
  }
}

}  // namespace model

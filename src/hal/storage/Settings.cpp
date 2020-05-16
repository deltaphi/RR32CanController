#include "hal/storage/Settings.h"

#include "SPIFFS.h"

namespace hal {
namespace storage {

const char* Settings::kSettingsFilename = "/settings.prefs";

size_t Settings::loadData(application::model::Settings* data) {
  if (data == nullptr) {
    return 0;
  }

  File f = SPIFFS.open(kSettingsFilename);
  if (!f) {
    printf("Opening '%s' for reading failed.\n", kSettingsFilename);
    return 0;
  } else {
    size_t readBytes = f.read(reinterpret_cast<uint8_t*>(data), sizeof(application::model::Settings));
    f.close();
    printf("%s: Loaded %i bytes.\n", kSettingsFilename, readBytes);
    return readBytes;
  }
}

bool Settings::load(application::model::Settings* data) { return loadData(data) != 0; }

void Settings::store(const application::model::Settings* const data) {
  application::model::Settings tmpData;
  size_t readBytes = loadData(&tmpData);

  if (readBytes != sizeof(application::model::Settings) || memcmp(&data, &tmpData, sizeof(application::model::Settings) != 0)) {
    // We need to store
    File f = SPIFFS.open(kSettingsFilename, "w");

    if (!f) {
      printf("Opening '%s' for writing failed.\n", kSettingsFilename);
    } else {
      size_t writtenBytes =
          f.write(reinterpret_cast<const uint8_t*>(data), sizeof(application::model::Settings));
      printf("%s: Wrote %i/%i bytes.\n", kSettingsFilename, writtenBytes,
             sizeof(application::model::Settings));

      f.close();
    }
  } else {
    printf("No settings change.\n");
  }
}

}  // namespace storage
}  // namespace hal

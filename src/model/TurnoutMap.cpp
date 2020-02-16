#include "model/TurnoutMap.h"

#include "SPIFFS.h"

namespace model {

const char* TurnoutMap::kTurnoutMapSettingsFile = "/turnoutmap.prefs";

void TurnoutMap::begin() {
  for (int i = 0; i < TURNOUT_MAP_LENGTH; ++i) {
    turnoutMap[i].mode = TurnoutAddressMode::SingleTurnout;
    turnoutMap[i].address = i;
  }

  load();
}

bool TurnoutMap::load() {
  File f = SPIFFS.open(kTurnoutMapSettingsFile, "r");
  if (!f) {
    printf("Opening '%s' for reading failed.\n", kTurnoutMapSettingsFile);
    return false;
  } else {
    size_t readBytes =
        f.read(reinterpret_cast<uint8_t*>(turnoutMap), sizeof(TurnoutMap_t));
    f.close();
    printf("%s: Loaded %i bytes.\n", kTurnoutMapSettingsFile, readBytes);
    return true;
  }
}

void TurnoutMap::store() {
  File f = SPIFFS.open(kTurnoutMapSettingsFile, "w");

  if (!f) {
    printf("Opening '%s' for writing failed.\n", kTurnoutMapSettingsFile);
  } else {
    size_t writtenBytes =
        f.write(reinterpret_cast<uint8_t*>(turnoutMap), sizeof(TurnoutMap_t));
    printf("%s: Wrote %i/%i bytes.\n", kTurnoutMapSettingsFile, writtenBytes,
           sizeof(TurnoutMap_t));

    f.close();
  }
}

void TurnoutMap::print() const {
  for (int i = 0; i < TURNOUT_MAP_LENGTH; ++i) {
    printf("%i:%i ", i,
           RR32Can::HumanTurnoutAddress(turnoutMap[i].address).value());
  }
  printf("\n");
}

model::TurnoutLookupResult& TurnoutMap::rangeCheckedMapAt(
    ButtonIndex_t button) {
  button = buttonToArrayIndex(button);

  if (button > TURNOUT_MAP_LENGTH) {
    printf("ERROR: Accessing turnoutMap out of bounds (%i) at %i.\n",
           TURNOUT_MAP_LENGTH, button);
#if (TURNOUT_MAP_LENGTH > 0)
    button = TURNOUT_MAP_LENGTH - 1;
#else
    button = 0;
#endif
  }

  return turnoutMap[button];
}

model::TurnoutLookupResult TurnoutMap::lookupTurnout(
    uint8_t buttonIndex) const {
#if (LOG_BUTTON_MAPPING == STD_ON)
  printf("Mapping from %i to ", buttonIndex);
  model::print(rangeCheckedMapAt(buttonIndex));
  printf(".\n");
#endif

  return rangeCheckedMapAt(buttonIndex);
}

void TurnoutMap::setLookupTurnout(ButtonIndex_t buttonIndex,
                                  model::TurnoutLookupResult newResult) {
  rangeCheckedMapAt(buttonIndex) = newResult;
}

}  // namespace model

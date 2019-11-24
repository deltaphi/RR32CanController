#include "model/TurnoutMap.h"

namespace model {

void TurnoutMap::begin() {
  for (int i = 0; i < TURNOUT_MAP_LENGTH; ++i) {
    turnoutMap[i].mode = model::TurnoutAddressMode::SingleTurnout;
    turnoutMap[i].address = i;
  }
}

void TurnoutMap::print() const {
  for (int i = 0; i < TURNOUT_MAP_LENGTH; ++i) {
    printf("%i:%i ", i, turnoutMap[i].address);
  }
  printf("\n");
}

model::TurnoutLookupResult& TurnoutMap::rangeCheckedMapAt(
    ButtonIndex_t button) {
  button = buttonToArrayIndex(button);

  if (button > TURNOUT_MAP_LENGTH) {
    printf("ERROR: Accessing turnoutMap out of bounds (%i) at %i.\n",
           TURNOUT_MAP_LENGTH, button);
    button = TURNOUT_MAP_LENGTH - 1;
  }

  return turnoutMap[button];
}

model::TurnoutLookupResult TurnoutMap::lookupTurnout(
    uint8_t buttonIndex) const {
#if (LOG_BUTTON_MAPPING == STD_ON)
  Serial.print("Mapping from ");
  Serial.print(buttonIndex, DEC);
  Serial.print(" to ");
  Serial.print(rangeCheckedMapAt(buttonIndex).address, DEC);
  Serial.println();
#endif

  return rangeCheckedMapAt(buttonIndex);
}

void TurnoutMap::setLookupTurnout(ButtonIndex_t buttonIndex,
                                  model::TurnoutLookupResult newResult) {
  rangeCheckedMapAt(buttonIndex) = newResult;
}

}  // namespace model

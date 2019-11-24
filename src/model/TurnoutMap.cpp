#include "model/TurnoutMap.h"

namespace model {

void TurnoutMap::begin() {
  for (int i = 0; i < TURNOUT_MAP_LENGTH; ++i) {
    turnoutMap[i].mode = model::TurnoutAddressMode::SingleTurnout;
    turnoutMap[i].address = i;
  }
}

model::TurnoutLookupResult TurnoutMap::lookupTurnout(
    uint8_t buttonIndex) const {
  buttonIndex -= TURNOUT_BUTTONS_OFFSET;  // Remove the offset

  buttonIndex /= 2;  // Ignore the lowest bit

#if (LOG_BUTTON_MAPPING == STD_ON)
  Serial.print("Mapping from ");
  Serial.print(buttonIndex, DEC);
#endif

  if (buttonIndex > TURNOUT_MAP_LENGTH) {
    printf("Accessing turnoutMap out of bounds (%i) at %i.", TURNOUT_MAP_LENGTH,
           buttonIndex);
    buttonIndex = TURNOUT_MAP_LENGTH - 1;
  }

#if (LOG_BUTTON_MAPPING == STD_ON)
  Serial.print(" to ");
  Serial.print(turnoutMap[buttonIndex].address, DEC);
  Serial.println();
#endif

  return turnoutMap[buttonIndex];
}

}  // namespace model

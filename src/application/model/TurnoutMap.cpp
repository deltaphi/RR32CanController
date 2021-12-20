#include "application/model/TurnoutMap.h"

#include <cstdio>

namespace application {
namespace model {

void TurnoutMap::begin() {
  for (RR32Can::MachineTurnoutAddress::value_type i = 0; i < TURNOUT_MAP_LENGTH; ++i) {
    turnoutMap[i].mode = application::model::TurnoutAddressMode::SingleTurnout;
    turnoutMap[i].address = RR32Can::MachineTurnoutAddress{i};
  }
}

void TurnoutMap::print() const {
  for (int i = 0; i < TURNOUT_MAP_LENGTH; ++i) {
    printf("%i:%i ", i, RR32Can::HumanTurnoutAddress(turnoutMap[i].address).value());
  }
  printf("\n");
}

application::model::TurnoutLookupResult& TurnoutMap::rangeCheckedMapAt(ButtonIndex_t button) {
  button = buttonToArrayIndex(button);

  if (button > TURNOUT_MAP_LENGTH) {
    printf("ERROR: Accessing turnoutMap out of bounds (%i) at %i.\n", TURNOUT_MAP_LENGTH, button);
    button = TURNOUT_MAP_LENGTH - 1;
  }

  return turnoutMap[button];
}

application::model::TurnoutLookupResult TurnoutMap::lookupTurnout(uint8_t buttonIndex) const {
#if (LOG_BUTTON_MAPPING == STD_ON)
  printf("Mapping from %i to ", buttonIndex);
  model::print(rangeCheckedMapAt(buttonIndex));
  printf(".\n");
#endif

  return rangeCheckedMapAt(buttonIndex);
}

void TurnoutMap::setLookupTurnout(ButtonIndex_t buttonIndex,
                                  application::model::TurnoutLookupResult newResult) {
  rangeCheckedMapAt(buttonIndex) = newResult;
}

}  // namespace model
}  // namespace application

#include "TurnoutTypes.h"
#include "config.h"

namespace TurnoutControl {

// Action lists use human numbering
ActionList actionLists[NumActionLists] = {
    {{20, TurnoutDirection::RED},
     {4, TurnoutDirection::GREEN},
     {5, TurnoutDirection::GREEN}},  // 1 red
    {{20, TurnoutDirection::RED},
     {4, TurnoutDirection::GREEN},
     {5, TurnoutDirection::RED}},  // 2 red
    {{20, TurnoutDirection::RED},
     {4, TurnoutDirection::RED},
     {250, TurnoutDirection::GREEN}},  // 3 red. Last switch is a "don't care"
    {{20, TurnoutDirection::GREEN},
     {7, TurnoutDirection::GREEN},
     {250, TurnoutDirection::RED}},  // 1 green. Last switch is a "don't care"
    {{20, TurnoutDirection::GREEN},
     {7, TurnoutDirection::RED},
     {22, TurnoutDirection::RED}},  // 2 green
    {{20, TurnoutDirection::GREEN},
     {7, TurnoutDirection::RED},
     {22, TurnoutDirection::GREEN}},  // 3 green
};

TurnoutLookupResult lookupTurnout(uint8_t buttonIndex) {
  buttonIndex /= 2;  // Ignore the lowest bit

#if (LOG_BUTTON_MAPPING == STD_ON)
  Serial.print("Mapping from ");
  Serial.print(buttonIndex, DEC);
#endif

  // We now get the following values for the hardware:
  // 16 17 18 19  20 21 22 23   8  9 10 11  12 13 14 15   0  1  2  3

  TurnoutLookupResult result;
  result.mode = TurnoutAddressMode::SingleTurnout;

  switch (buttonIndex) {
    case 16:
      result.address = 39;
      break;
    case 17:
      result.address = 40;
      break;
    case 18:
      result.address = 41;
      break;
    case 19:
      result.address = 42;
      break;

    case 20:
      result.address = 43;
      break;
    case 21:
      result.address = 44;
      break;
    case 22:
      result.address = 45;
      break;
    case 23:
      result.address = 46;
      break;

    case 8:
      result.address = 47;
      break;
    case 9:
      result.address = 48;
      break;
    case 10:
      result.address = 11;
      break;
    case 11:
      result.address = 27;
      break;

    case 12:
      result.address = 9;
      break;
    case 13:
      result.address = 49;
      break;
    case 14:
      result.address = 50;
      break;
    case 15:
      result.address = 51;
      break;

    case 0:
      result.address = 59;
      break;
    case 1:
      // Multi-Turnout
      result.mode = TurnoutAddressMode::MultiTurnout;
      result.address = 1;
      break;
    case 2:
      // Multi-Turnout
      result.mode = TurnoutAddressMode::MultiTurnout;
      result.address = 2;
      break;
    case 3:
      // Multi-Turnout
      result.mode = TurnoutAddressMode::MultiTurnout;
      result.address = 3;
      break;

    default:
      result.address = buttonIndex;
  }

#if (LOG_BUTTON_MAPPING == STD_ON)
  Serial.print(" to ");
  Serial.print(buttonIndex, DEC);
  Serial.println();
#endif

  return result;
}

} /* namespace TurnoutControl */
#ifndef __TURNOUT_TYPES_H__
#define __TURNOUT_TYPES_H__

#include <Arduino.h>

#include "RR32Can/constants.h"

namespace TurnoutControl {

enum class TurnoutAddressMode { SingleTurnout = 0, MultiTurnout };

typedef struct {
  TurnoutAddressMode mode = TurnoutAddressMode::SingleTurnout;
  uint8_t address = 0;
} TurnoutLookupResult;

typedef struct {
  uint8_t address;
  RR32Can::TurnoutDirection direction;
} TurnoutAction;

constexpr uint8_t NumActions = 3;

using ActionList = TurnoutAction[NumActions];

constexpr uint8_t NumActionLists = 6;

TurnoutLookupResult lookupTurnout(uint8_t buttonIndex);

extern ActionList actionLists[NumActionLists];

} /* namespace TurnoutControl */

#endif
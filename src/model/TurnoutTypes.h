#ifndef __TURNOUT_TYPES_H__
#define __TURNOUT_TYPES_H__

#include <Arduino.h>

#include "RR32Can/Types.h"

namespace model {

enum class TurnoutAddressMode { SingleTurnout = 0, MultiTurnout };

typedef struct {
  TurnoutAddressMode mode = TurnoutAddressMode::SingleTurnout;
  RR32Can::TurnoutAddress_t address = 0;
} TurnoutLookupResult;

typedef struct {
  RR32Can::TurnoutAddress_t address;
  RR32Can::TurnoutDirection direction;
} TurnoutAction;

constexpr uint8_t NumActions = 3;

using ActionList = TurnoutAction[NumActions];

constexpr uint8_t NumActionLists = 6;

} /* namespace model */

namespace TurnoutControl {

extern model::ActionList actionLists[model::NumActionLists];

} /* namespace TurnoutControl */

#endif

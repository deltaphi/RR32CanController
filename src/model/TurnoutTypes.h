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

void print(const TurnoutLookupResult&);

typedef struct {
  RR32Can::TurnoutAddress_t address;
  RR32Can::TurnoutDirection direction;
} TurnoutAction;

void print(const TurnoutAction&);

} /* namespace model */

#endif

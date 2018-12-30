#ifndef __TURNOUT_TYPES_H__
#define __TURNOUT_TYPES_H__

#include <Arduino.h>

enum class TurnoutAddressMode { SingleTurnout = 0, MultiTurnout };

typedef struct {
  TurnoutAddressMode mode = TurnoutAddressMode::SingleTurnout;
  uint8_t address = 0;
} TurnoutLookupResult;

enum class TurnoutDirection: uint8_t { RED = 0, GREEN = 1 };

typedef struct {
  uint8_t address;
  TurnoutDirection direction;
} TurnoutAction;

constexpr uint8_t NumActions = 3;

using ActionList = TurnoutAction[NumActions];

constexpr uint8_t NumActionLists = 6;

TurnoutLookupResult lookupTurnout(uint8_t buttonIndex);

extern ActionList actionLists[NumActionLists];
#endif
#ifndef __TURNOUTCONTROL_TURNOUTCONTROL_H__
#define __TURNOUTCONTROL_TURNOUTCONTROL_H__


#include <Arduino.h>
#include <AsyncShiftIn.h>

#include "ActionListProcessor.h"
#include "DebouncedDualKey.h"
#include "TurnoutTypes.h"
#include "config.h"

namespace TurnoutControl {

/// Whether the shift register has been initialized
extern boolean initialized;
extern AsyncShiftIn shiftRegister0;

using DebouncedKeyType = DebouncedKey<10, 0>;
using DebouncedKeyArray = DebouncedKeyType[SHIFT_REGISTER_LENGTH];

extern DebouncedKeyArray keyArray;
extern ActionListProcessor actionListProcessor;

void begin();
void loop();

void sendTurnoutPacket(uint32_t turnoutAddress, TurnoutDirection direction, uint8_t power);
void handleButton(uint8_t buttonIndex, uint8_t buttonState);
void handleMultiturnout(TurnoutLookupResult result, TurnoutDirection requestedDirection);

} /* namespace TurnoutControl */

#endif
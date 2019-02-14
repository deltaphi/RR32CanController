#ifndef __ENGINECONTROL_ENGINGECONTROL_H__
#define __ENGINECONTROL_ENGINGECONTROL_H__

#include <Arduino.h>
#include <CAN.h>

#if (ENCODER_ENABLED == STD_ON)
#include "RotaryEncoder.h"
#endif

#include "maerklin-can.h"
#include "config.h"

namespace EngineControl {

#if (ENCODER_ENABLED == STD_ON)
extern RotaryEncoder encoder;
void loopEncoder();

enum class DisplayMode {
  ENGINE,
  SELECT_ENGINE
};
#endif

void begin();
void loop();


} /* namespace EngineControl */

#endif

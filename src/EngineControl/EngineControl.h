#ifndef __ENGINECONTROL_ENGINGECONTROL_H__
#define __ENGINECONTROL_ENGINGECONTROL_H__

#include <Arduino.h>

#if (ENCODER_ENABLED == STD_ON)
#include "RotaryEncoder.h"
#endif

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

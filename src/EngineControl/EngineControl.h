#ifndef __ENGINECONTROL_ENGINGECONTROL_H__
#define __ENGINECONTROL_ENGINGECONTROL_H__

#include <Arduino.h>

#if (ENCODER_ENABLED == STD_ON)
#include "RotaryEncoder.h"
#endif

#include "config.h"
#include "RR32Can/Types.h"

#if (DISPLAY_ATTACHED == STD_ON)
#include "EngineControl/DisplayManager.h"
#endif

namespace EngineControl {

#if (DISPLAY_ATTACHED == STD_ON)
extern DisplayManager displayManager;
#endif

#if (ENCODER_ENABLED == STD_ON)
using RotaryEngoderTick_t = long;
extern RotaryEncoder encoder;
void loopEncoder();
void loopButtons();

/**
 * \brief set the encoder value when the state is ENGINE
 */
void setEngineVelocity(RR32Can::Velocity_t velocity);

enum class DisplayMode { ENGINE, SELECT_ENGINE };
#endif

void begin();
void loop();

} /* namespace EngineControl */

#endif

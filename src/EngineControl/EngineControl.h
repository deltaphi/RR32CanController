#ifndef __ENGINECONTROL_ENGINGECONTROL_H__
#define __ENGINECONTROL_ENGINGECONTROL_H__

#include <Arduino.h>
#include <CAN.h>

#if (DISPLAY_ATTACHED == STD_ON)
#include <Wire.h>
#include "SSD1306Wire.h"
#endif

#if (ENCODER_ENABLED == STD_ON)
#include "RotaryEncoder.h"
#endif

#include "maerklin-can.h"
#include "config.h"

namespace EngineControl {

#if (DISPLAY_ATTACHED == STD_ON)
extern SSD1306Wire display;
void loopDisplay();

#define STRING_CHAR_LENGTH (16)
#define STRING_DATATYPE_LENGTH ((STRING_CHAR_LENGTH) + 1)

using TextBuffer = char[2][STRING_DATATYPE_LENGTH];

extern TextBuffer displayText;
extern TextBuffer displayTextBuffer;

#endif

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

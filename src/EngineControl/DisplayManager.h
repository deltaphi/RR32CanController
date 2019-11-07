#ifndef __ENGINECONTROL_DISPLAYMANAGER_H__
#define __ENGINECONTROL_DISPLAYMANAGER_H__

#include <Arduino.h>
#include "RR32Can/Types.h"
#include "config.h"

#if (DISPLAY_ATTACHED == STD_ON)

#include "SSD1306Wire.h"

namespace EngineControl {

#define STRING_CHAR_LENGTH (16)
#define STRING_DATATYPE_LENGTH ((STRING_CHAR_LENGTH) + 1)
#define DISPLAY_LINES (2)

class DisplayManager {
 public:
  using LineBuffer = char[STRING_DATATYPE_LENGTH];

  void begin();

  /**
   * \brief Return a buffer to be written to and set the flag that a change
   * needs to be printed in the next loop.
   *
   * \param lineNumber The index of the line (0-based) to be rewritten.
   * Out-of-bounds values will wrap.
   */
  LineBuffer& getWritableBuffer(uint8_t lineNumber) {
    updateRequired = true;
    return buffer[lineNumber % DISPLAY_LINES];
  }

  void updateBuffer(const char* data, uint8_t dataLen, uint8_t lineNumber);

  void loop();

  void enableCursor() {
    if (!cursorEnabled) {
      cursorEnabled = true;
      updateRequired = true;
    }
  }

  void disableCursor() {
    if (cursorEnabled) {
      cursorEnabled = false;
      updateRequired = true;
    }
  }

  void setCursorLine(uint8_t line) {
    if (this->cursorLine != line) {
      this->cursorLine = line;
      updateRequired = true;
    }
  }

  uint8_t getCursorLine() const { return cursorLine; }

  /**
   * \brief Set the current speed value to be displayed. Value is mapped to
   * 0..100 range.
   */
  void setSpeedValue(RR32Can::Velocity_t speed) {
    if (speed > 1000) {
      speed = 1000;
    }
    if (speed != this->speed) {
      this->speed = speed;
      updateRequired = true;
    }
  }

  void setDirection(RR32Can::EngineDirection direction) {
    if (this->direction != direction) {
      this->direction = direction;
      updateRequired = true;
    }
  }

  void setWifi(bool onOff) {
    if (wifiOn != onOff) {
      wifiOn = onOff;
      updateRequired = true;
    }
  }

  void setCan(bool onOff) {
    if (canOn != onOff) {
      canOn = onOff;
      updateRequired = true;
    }
  }

  void setFunctionBits(uint8_t functionBits) {
    if (this->functionBits != functionBits) {
      this->functionBits = functionBits;
      updateRequired = true;
    }
  }

 private:
  bool cursorEnabled;
  uint8_t cursorLine;
  static const uint8_t voffset[];
  static const uint8_t baselineOffset[];

  uint8_t speed;
  uint8_t functionBits;
  RR32Can::EngineDirection direction;

  bool wifiOn = true;
  bool canOn = true;

  using TextBuffer = LineBuffer[DISPLAY_LINES];

  SSD1306Wire display = {DISPLAY_TWI_ADDRESS, TWI_SDA_PIN, TWI_SCL_PIN};
  TextBuffer buffer;
  bool updateRequired = false;
};

} /* namespace EngineControl */

#endif /* Display attached */
#endif /* header guard */

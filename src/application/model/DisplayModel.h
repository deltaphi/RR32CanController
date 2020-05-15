#ifndef __APPLICATION__MODEL__DISPLAYMODEL_H__
#define __APPLICATION__MODEL__DISPLAYMODEL_H__

#include <cstdint>

#include "RR32Can/Types.h"

#define STRING_CHAR_LENGTH (16)
#define STRING_DATATYPE_LENGTH ((STRING_CHAR_LENGTH) + 1)
#define DISPLAY_LINES (2)

namespace hal {
  // Forward declaration for friend declaration
  class DisplayDriver;
}

namespace application {
namespace model {

/*
 * \brief Class DisplayModel
 */
class DisplayModel {
  friend class hal::DisplayDriver;
 public:
  using LineBuffer = char[STRING_DATATYPE_LENGTH];

  void begin();

  void updateBuffer(const char* data, uint8_t dataLen, uint8_t lineNumber);

  bool isUpdateRequired() const {
    return updateRequired;
  }

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

  bool isCursorEnabled() const {
    return cursorEnabled;
  }

  void setCursorLine(uint8_t line) {
    if (this->cursorLine != line) {
      this->cursorLine = line;
      updateRequired = true;
    }
  }

  uint8_t getCursorLine() const { return cursorLine; }

  /**
   * \brief Set the current speed value to be displayed. Value must be in
   * 0..100 range.
   */
  void setSpeedValue(uint8_t speed) {
    if (speed > 100) {
      speed = 100;
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

  bool isWifi() const {
    return wifiOn;
  }

  void setCan(bool onOff) {
    if (canOn != onOff) {
      canOn = onOff;
      updateRequired = true;
    }
  }

  bool isCan() const {
    return canOn;
  }

  void setFunctionBits(uint8_t functionBits) {
    if (this->functionBits != functionBits) {
      this->functionBits = functionBits;
      updateRequired = true;
    }
  }

  void setSystem(bool onOff) {
    if (systemOn != onOff) {
      systemOn = onOff;
      updateRequired = true;
    }
  }

  bool getSystemOn() const { return systemOn; }

 private:
  bool cursorEnabled;
  uint8_t cursorLine;

  RR32Can::Velocity_t speed;
  RR32Can::FunctionBits_t functionBits;
  RR32Can::EngineDirection direction;

  bool wifiOn = false;
  bool canOn = false;
  bool systemOn = true;

  using TextBuffer = LineBuffer[DISPLAY_LINES];

  TextBuffer buffer;
  bool updateRequired = false;
};

}  // namespace model
}  // namespace application

#endif  // __APPLICATION__MODEL__DISPLAYMODEL_H__

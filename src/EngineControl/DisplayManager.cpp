#include "config.h"

#if (DISPLAY_ATTACHED == STD_ON)

#include "EngineControl/DisplayManager.h"

namespace EngineControl {

void DisplayManager::begin() {
  Serial.println("Starting Display");
  // Initialize the Display
  display.init();
  display.flipScreenVertically();

  memset(buffer, 0, sizeof(buffer));
  strncpy(getWritableBuffer(0), "Encoder:", STRING_CHAR_LENGTH);
}

void DisplayManager::loop() {
  if (updateRequired) {
    updateRequired = false;
    display.clear();

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);

    for (uint8_t line = 0; line < DISPLAY_LINES; ++line) {
      display.drawString(0, voffset[line], buffer[line]);
    }

    display.display();
  }
}

} /* namespace EngineControl */

#endif
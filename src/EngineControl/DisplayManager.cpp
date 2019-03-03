#include "config.h"

#if (DISPLAY_ATTACHED == STD_ON)

#include "EngineControl/DisplayManager.h"

namespace EngineControl {

const uint8_t DisplayManager::voffset[] = {0, 26};
const uint8_t DisplayManager::baselineOffset[] = {24, 50};

void DisplayManager::begin() {
  Serial.println("Starting Display");
  // Initialize the Display
  display.init();
  display.flipScreenVertically();

  memset(buffer, 0, sizeof(buffer));
}

void DisplayManager::loop() {
  if (updateRequired) {
    updateRequired = false;
    display.clear();

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);

    for (uint8_t line = 0; line < DISPLAY_LINES; ++line) {
      display.drawString(0, voffset[line], buffer[line]);
      if (cursorEnabled && cursorLine == line) {
        display.drawHorizontalLine(0, baselineOffset[line], display.getWidth());
      }
    }

    display.display();
  }
}

} /* namespace EngineControl */

#endif
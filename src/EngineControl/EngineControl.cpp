#include <CAN.h>

#include "DebouncedDualKey.h"
#include "EngineControl/DisplayManager.h"
#include "EngineControl/EngineControl.h"
#include "config.h"

#include "MaerklinCan/constants.h"
#include "MaerklinCan/handler.h"

namespace EngineControl {

#if (DISPLAY_ATTACHED == STD_ON)
DisplayManager displayManager;
DisplayMode displayMode = DisplayMode::ENGINE;
#endif

#if (ENCODER_ENABLED == STD_ON)
RotaryEncoder encoder(ENCODER_A_PIN, ENCODER_B_PIN);
int encoderPosition = 0;

DebouncedKey<1, 1> encoderKey;
#endif

void begin() {
#if (DISPLAY_ATTACHED == STD_ON)
  displayManager.begin();
#endif

#if (ENCODER_ENABLED == STD_ON)
  Serial.println("Starting Encoder");
  pinMode(ENCODER_BUTTON_PIN, INPUT);

  encoderKey.forceDebounce(HIGH);
  encoderKey.getAndResetEdgeFlag();
#if (DISPLAY_ATTACHED == STD_ON)
  snprintf(displayManager.getWritableBuffer(1), STRING_CHAR_LENGTH, "%d",
           encoderPosition);
#endif
#endif
}

void loop() {
#if (DISPLAY_ATTACHED == STD_ON)
  displayManager.loop();
#endif

#if (ENCODER_ENABLED == STD_ON)
  loopEncoder();
#endif
}

void sendQueryEngineName(uint8_t offset) {
  Serial.print("Querying for Engines...");

  MaerklinCan::SendRequestConfigDataPacket("loknamen", 8);

  char buffer[MaerklinCan::CanDataMaxLength + 1];
  uint8_t printedCharCount =
      snprintf(buffer, MaerklinCan::CanDataMaxLength, "%d %d", offset, 2);
  MaerklinCan::SendRequestConfigDataPacket(buffer, printedCharCount);

  Serial.println(" done.");
}

#if (ENCODER_ENABLED == STD_ON)

void loopEncoder() {
  uint8_t readButton = digitalRead(ENCODER_BUTTON_PIN);
  encoderKey.cycle(readButton);
  if (encoderKey.getAndResetEdgeFlag()) {
    // There was an edge
    if (encoderKey.getDebouncedValue() == LOW) {
      Serial.println("Pressed Encoder Button");

      // Switch Menu mode
      if (displayMode == DisplayMode::ENGINE) {
        displayMode = DisplayMode::SELECT_ENGINE;
        sendQueryEngineName(0);
      } else {
        displayMode = DisplayMode::ENGINE;
      }
    } else {
      Serial.println("Released Encoder Button");
    }
  }

  encoder.tick();

  int newPosition = encoder.getPosition();
  if (encoderPosition != newPosition) {
    Serial.print("Encoder position: ");
    Serial.print(newPosition);
    Serial.println();
    encoderPosition = newPosition;

#if (DISPLAY_ATTACHED == STD_ON)
    snprintf(displayManager.getWritableBuffer(1), STRING_CHAR_LENGTH, "%d",
             encoderPosition);
#endif
  }
}
#endif

} /* namespace EngineControl */

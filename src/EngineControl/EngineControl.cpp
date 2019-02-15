#include <CAN.h>
#include "maerklin-can.h"

#include "EngineControl/EngineControl.h"
#include "config.h"
#include "DebouncedDualKey.h"
#include "EngineControl/DisplayManager.h"

#include "MaerklinCan/Identifier.h"
#include "MaerklinCan/Constants.h"
#include "MaerklinCan/Data.h"


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
  snprintf(displayManager.getWritableBuffer(1), STRING_CHAR_LENGTH, "%d", encoderPosition);
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
  // Just try to download the first two engines from the MS2
  MaerklinCan::Identifier identifier;
  // identifier.prio = 4; // Value is specified but actual implementations don't
  // use it.
  identifier.command = MaerklinCan::kRequestConfigData;
  identifier.response = false;
  identifier.computeAndSetHash(maerklinCanUUID);

  // Don't bother with a data struct here.
  constexpr static uint8_t charCount = 8;
  constexpr static char kLocoNames[charCount + 1] = "loknamen";

  // Send packet on CAN
  CAN.beginExtendedPacket(identifier.makeIdentifier());
  for (int i = 0; i < charCount; ++i) {
    CAN.write(kLocoNames[i]);
  }
  CAN.endPacket();

  CAN.beginExtendedPacket(identifier.makeIdentifier());
  char buffer[charCount + 1];
  uint8_t printedCharCount = snprintf(buffer, charCount, "%d %d", offset, 2);
  for (int i = 0; i < printedCharCount; ++i) {
    CAN.write(buffer[i]);
  }
  CAN.endPacket();

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
        snprintf(displayManager.getWritableBuffer(1), STRING_CHAR_LENGTH, "%d", encoderPosition);
#endif
    }
}
#endif

} /* namespace EngineControl */

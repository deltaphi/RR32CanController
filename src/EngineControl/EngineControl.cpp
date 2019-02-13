#include "EngineControl/EngineControl.h"
#include "config.h"
#include "DebouncedDualKey.h"

namespace EngineControl {

#if (DISPLAY_ATTACHED == STD_ON)
SSD1306Wire display(DISPLAY_TWI_ADDRESS, TWI_SDA_PIN, TWI_SCL_PIN);

DisplayMode displayMode = DisplayMode::ENGINE;

boolean displayUpdateRequested = false;
TextBuffer displayText;

constexpr const uint8_t voffset[] = {0, 26};
#endif

#if (ENCODER_ENABLED == STD_ON)
RotaryEncoder encoder(ENCODER_A_PIN, ENCODER_B_PIN);
int encoderPosition = 0;

DebouncedKey<1, 1> encoderKey;
#endif

void begin() {

#if (DISPLAY_ATTACHED == STD_ON)
  Serial.println("Starting Display");
  // Initialize the Display
  display.init();
  display.flipScreenVertically();

  memset(displayText, 0, sizeof(displayText));
  strncpy(displayText[0], "Encoder:", STRING_CHAR_LENGTH);
  snprintf(displayText[1], STRING_CHAR_LENGTH, "%d", encoderPosition);
  displayUpdateRequested = true;
#endif

#if (ENCODER_ENABLED == STD_ON)
  Serial.println("Starting Encoder");
  pinMode(ENCODER_BUTTON_PIN, INPUT);

  encoderKey.forceDebounce(HIGH);
  encoderKey.getAndResetEdgeFlag();
#endif
}

void loop() {
#if (DISPLAY_ATTACHED == STD_ON)
  loopDisplay();
#endif

#if (ENCODER_ENABLED == STD_ON)
  loopEncoder();
#endif
}


#if (DISPLAY_ATTACHED == STD_ON)

void printLine(uint8_t line) {
  display.drawString(0, voffset[line], displayText[line]);
}

void loopDisplay() {
  if (displayUpdateRequested) {
    displayUpdateRequested = false;
    display.clear();

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_24);

    printLine(0);
    printLine(1);

    display.display();
  }
}
#endif

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
        
        snprintf(displayText[1], STRING_CHAR_LENGTH, "%d", encoderPosition);
        displayUpdateRequested = true;
    }
}
#endif

} /* namespace EngineControl */

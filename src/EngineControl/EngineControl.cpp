#include <Arduino.h>
#include <CAN.h>

#include "DebouncedDualKey.h"
#include "EngineControl/DisplayManager.h"
#include "EngineControl/EngineControl.h"
#include "config.h"

#include "RR32Can/RR32Can.h"
#include "RR32Can/constants.h"

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
  strncpy(displayManager.getWritableBuffer(0), "Encoder:", STRING_CHAR_LENGTH);
#endif

#if (ENCODER_ENABLED == STD_ON)
  Serial.println("Starting Encoder");
  pinMode(ENCODER_BUTTON_PIN, INPUT);

  encoderKey.forceDebounce(HIGH);
  encoderKey.getAndResetEdgeFlag();

#endif
}

void setDisplayPending() {
  for (uint8_t i = 0; i < DISPLAY_LINES; ++i) {
    strncpy(displayManager.getWritableBuffer(i), "...", STRING_CHAR_LENGTH);
  }
}

void startDisplayModeSelectEngine() {
  displayMode = DisplayMode::SELECT_ENGINE;
  RR32Can::EngineBrowser& engineBrowser = RR32Can::RR32Can.getEngineBrowser();
  engineBrowser.reset();
  RR32Can::RR32Can.RequestEngineList(0);
  setDisplayPending();
}

void displayModeSelectEngineLoop() {
  if (RR32Can::RR32Can.getConfigStreamState() ==
      RR32Can::ConfigDataStreamParser::StreamState::STREAM_DONE) {
    RR32Can::EngineBrowser& engineBrowser = RR32Can::RR32Can.getEngineBrowser();
    Serial.print("Engine Browser done, copying line to display:");
    // Copy interesting entries to display
    uint8_t line = 0;
    for (const RR32Can::EngineShortInfo& info :
         engineBrowser.getEngineInfos()) {
      DisplayManager::LineBuffer& buffer =
          displayManager.getWritableBuffer(line);
      strncpy(&buffer[0], info.getName(), STRING_DATATYPE_LENGTH);
      Serial.print(" '");
      Serial.print(&buffer[0]);
      Serial.print("'");
      ++line;
    }
    RR32Can::RR32Can.notifyConfigStreamReceived();
    Serial.println();
  }
}

void startDisplayModeEngine() {
  displayMode = DisplayMode::ENGINE;
#if (DISPLAY_ATTACHED == STD_ON)
  strncpy(displayManager.getWritableBuffer(0), "Encoder:", STRING_CHAR_LENGTH);
#endif
}

void displayModeEngineLoop() {
#if (DISPLAY_ATTACHED == STD_ON)
  snprintf(displayManager.getWritableBuffer(1), STRING_CHAR_LENGTH, "%d",
           encoderPosition);
#endif
}

void loop() {
  if (displayMode == DisplayMode::SELECT_ENGINE) {
    displayModeSelectEngineLoop();
  } else {
    displayModeEngineLoop();
  }

#if (DISPLAY_ATTACHED == STD_ON)
  displayManager.loop();
#endif

#if (ENCODER_ENABLED == STD_ON)
  loopEncoder();
#endif
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
        startDisplayModeSelectEngine();
      } else {
        startDisplayModeEngine();
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

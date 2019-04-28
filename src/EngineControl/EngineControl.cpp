#include <Arduino.h>
#include <CAN.h>

#include "DebouncedDualKey.h"
#include "EngineControl/DisplayManager.h"
#include "EngineControl/EngineControl.h"
#include "config.h"

#include "RR32Can/RR32Can.h"
#include "RR32Can/Constants.h"

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

void forceEncoderPosition(uint8_t position) {
  encoderPosition = position;
  encoder.setPosition(position);
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
  forceEncoderPosition(0);
  displayManager.enableCursor();
  displayManager.setCursorLine(0);
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
  displayManager.disableCursor();
  forceEncoderPosition(0);
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

    if (displayMode == DisplayMode::ENGINE) {
#if (DISPLAY_ATTACHED == STD_ON)
      snprintf(displayManager.getWritableBuffer(1), STRING_CHAR_LENGTH, "%d",
               encoderPosition);
#endif
    } else {
      // SELECT_ENGINE. Move the cursor by one. If the cursor goes out of
      // bounds, request new data download.
      RR32Can::EngineBrowser& browser = RR32Can::RR32Can.getEngineBrowser();

      // int cursorPosition = encoderPosition - browser.getStreamOffset();

      int cursorPosition;

      if (encoderPosition < browser.getStreamOffset()) {
        // Try to go up in the list
        if (browser.getStreamOffset() == 0) {
          // can't go further. reject the change.
          cursorPosition = 0;
          forceEncoderPosition(cursorPosition);
        } else {
          RR32Can::RR32Can.RequestEngineList(browser.getStreamOffset() -
                                             RR32Can::kNumEngineNamesDownload);
          setDisplayPending();
          cursorPosition = encoderPosition - browser.getStreamOffset();
        }
      } else if (encoderPosition >=
                 browser.getStreamOffset() + RR32Can::kEngineBrowserEntries) {
        // Try to go down in the list

        if (encoderPosition >= browser.getNumEnginesKnownByMaster()) {
          forceEncoderPosition(browser.getNumEnginesKnownByMaster() - 1);
          cursorPosition = encoderPosition - browser.getStreamOffset();
        } else {
          RR32Can::RR32Can.RequestEngineList(browser.getStreamOffset() +
                                             RR32Can::kNumEngineNamesDownload);
          cursorPosition = encoderPosition - browser.getStreamOffset();
          setDisplayPending();
        }
      } else {
        cursorPosition = encoderPosition - browser.getStreamOffset();
      }

      displayManager.setCursorLine(cursorPosition);
    }
  }
}
#endif

} /* namespace EngineControl */

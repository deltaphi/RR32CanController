#include <Arduino.h>

#include "DebouncedDualKey.h"
#include "EngineControl/DisplayManager.h"
#include "EngineControl/EngineControl.h"
#include "config.h"

#include "RR32Can/Constants.h"
#include "RR32Can/RR32Can.h"
#include "RR32Can/Types.h"

namespace EngineControl {

#if (DISPLAY_ATTACHED == STD_ON)
DisplayManager displayManager;
DisplayMode displayMode = DisplayMode::ENGINE;
#endif

#if (ENCODER_ENABLED == STD_ON)
RotaryEncoder encoder(ENCODER_A_PIN, ENCODER_B_PIN);
int encoderPosition = 0;

DebouncedKey<1, 1> encoderKey;

DebouncedKey<1, 1> stopKey;
DebouncedKey<1, 1> shiftKey;
using FKeys_t = std::array<DebouncedKey<1, 1>, NUM_FBUTTONS>;
FKeys_t fKeys;
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

  pinMode(STOP_BUTTON_PIN, INPUT);
  stopKey.forceDebounce(HIGH);
  stopKey.getAndResetEdgeFlag();
  pinMode(SHIFT_BUTTON_PIN, INPUT);
  shiftKey.forceDebounce(HIGH);
  shiftKey.getAndResetEdgeFlag();

  pinMode(F0_BUTTON_PIN, INPUT);
  pinMode(F1_BUTTON_PIN, INPUT);
  pinMode(F2_BUTTON_PIN, INPUT);
  pinMode(F3_BUTTON_PIN, INPUT);
  // pinMode(F4_BUTTON_PIN, INPUT);

  for (FKeys_t::value_type& fkey : fKeys) {
    fkey.forceDebounce(HIGH);
    fkey.getAndResetEdgeFlag();
  }
#endif
}

void forceEncoderPosition(RotaryEngoderTick_t position) {
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
  strncpy(displayManager.getWritableBuffer(0), "...", STRING_CHAR_LENGTH);
  displayManager.disableCursor();
#endif
  forceEncoderPosition(0);
}

void displayModeEngineLoop() {
#if (DISPLAY_ATTACHED == STD_ON)
  RR32Can::EngineControl& engineControl = RR32Can::RR32Can.getEngineControl();
  RR32Can::Engine& engine = engineControl.getEngine();
  if (engine.isFullDetailsKnown()) {
    const char* engineName = engine.getName();
    displayManager.updateBuffer(engineName, STRING_CHAR_LENGTH, 0);

    const char* protocolString = engine.getProtocolString();
    RR32Can::Engine::Address_t engineAddress = engine.getAddress();
    char buf[STRING_DATATYPE_LENGTH];

    snprintf(buf, STRING_CHAR_LENGTH, "%s %i", protocolString, engineAddress);

    displayManager.updateBuffer(buf, STRING_CHAR_LENGTH, 1);

    displayManager.setDirection(engine.getDirection());
    displayManager.setSpeedValue(
        map(engine.getVelocity(), 0, RR32Can::kMaxEngineVelocity, 0, 100));
    displayManager.setFunctionBits(engine.getFunctionBits());
  } else {
    displayManager.updateBuffer("No Loco", STRING_CHAR_LENGTH, 0);
    displayManager.updateBuffer("RR32Can", STRING_CHAR_LENGTH, 1);
  }
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
  loopButtons();

  loopEncoder();
#endif
}

#if (ENCODER_ENABLED == STD_ON)

void loopEncoder() {
  // Read the encoder
  encoder.tick();
  RotaryEngoderTick_t newEncoderPosition = encoder.getPosition();

  uint8_t readButton = digitalRead(ENCODER_BUTTON_PIN);
  encoderKey.cycle(readButton);
  if (encoderKey.getAndResetEdgeFlag()) {
    // There was an edge
    if (encoderKey.getDebouncedValue() == LOW) {
      Serial.print("Pressed Encoder Button: Switch to ");

      if (shiftKey.getDebouncedValue() == LOW) {
        // Shift was pressed as well - switch menu mode.
        if (displayMode == DisplayMode::ENGINE) {
          Serial.println("ENGINE_SELECT");
          startDisplayModeSelectEngine();
        } else {
          // Abort selection and return to Engine Control with current Engine.
          Serial.println("ENGINE_CONTROL (Abort)");
          startDisplayModeEngine();
        }
      } else {
        RR32Can::EngineControl& control = RR32Can::RR32Can.getEngineControl();
        RR32Can::Engine& engine = control.getEngine();
        // Shift was not pressed - probably reverse direction?
        if (displayMode == DisplayMode::ENGINE) {
          if (engine.isFullDetailsKnown()) {
            // Send emergency stop for engine
            RR32Can::RR32Can.SendEmergencyStop(engine);
            // Send direction reverse for engine
            RR32Can::RR32Can.SendEngineDirection(
                engine, switchDirection(engine.getDirection()));
          }
        } else {
          // Commit the selected engine
          Serial.print("ENGINE_CONTROL (Commit).");
          RR32Can::EngineBrowser& browser = RR32Can::RR32Can.getEngineBrowser();
          uint8_t engineIndex = newEncoderPosition - browser.getStreamOffset();
          const RR32Can::EngineBrowser::EngineInfoSet& infoSet =
              browser.getEngineInfos();

          const RR32Can::EngineShortInfo& engineInfo = infoSet[engineIndex];
          if (engine.setNameConditional(engineInfo.getName())) {
            Serial.print(" Requesting Engine Data for '");
            Serial.print(engine.getName());
            Serial.println("'");
            RR32Can::RR32Can.RequestEngine(engine);
          } else {
            Serial.println();
          }
          browser.printAll();
          // Return to ENGINE control mode.
          startDisplayModeEngine();
        }
      }

    } else {
      Serial.println("Released Encoder Button");
    }
  }

  // See if there was an effective change to the reported encoder position
  if (encoderPosition != newEncoderPosition) {
    encoderPosition = newEncoderPosition;

    if (displayMode == DisplayMode::ENGINE) {
      // CONTROL ENGINE.
      // RR32Can::EngineControl& control = RR32Can::RR32Can.getEngineControl();

      // TBD: Move state to EngineController.
      // TBD: Initialize state only when a new engine is selected.

      // Limit newPosition to 0..128 and reset the encoder appropriately, if
      // necessary.
      if (encoderPosition < 0) {
        encoderPosition = 0;
        forceEncoderPosition(encoderPosition);
      }

      if (encoderPosition > RR32Can::kMaxEngineVelocity) {
        encoderPosition = RR32Can::kMaxEngineVelocity;
        forceEncoderPosition(encoderPosition);
      }

      // Update the engine velocity

      RR32Can::EngineControl& control = RR32Can::RR32Can.getEngineControl();
      RR32Can::Engine& engine = control.getEngine();

      if (engine.isFullDetailsKnown()) {
        RR32Can::Engine::Velocity_t oldVelocity = engine.getVelocity();
        if (encoderPosition != oldVelocity) {
          RR32Can::RR32Can.SendEngineVelocity(engine, encoderPosition);
        }
      }

    } else {
      // SELECT_ENGINE. Move the cursor by one. If the cursor goes out of
      // bounds, request new data download.
      RR32Can::EngineBrowser& browser = RR32Can::RR32Can.getEngineBrowser();

      // TBD: Move state to EngineBrowser instance.
      // TBD: Initialize state when engine browser is opened.

      int cursorPosition;

      if (encoderPosition < browser.getStreamOffset()) {
        // Try to go up in the list
        if (browser.getStreamOffset() == 0) {
          // can't go further. reject the change.
          cursorPosition = 0;
          forceEncoderPosition(cursorPosition);
        } else {
          browser.clearTable();
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
          browser.clearTable();
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

void checkAndPrint(DebouncedKey<1, 1>& key, const char* key_name,
                   uint8_t keyPin) {
  uint8_t readValue = digitalRead(keyPin);
  key.cycle(readValue);
  if (key.getAndResetEdgeFlag()) {
    Serial.print("Button '");
    Serial.print(key_name);
    Serial.print("' was ");
    if (key.getDebouncedValue() == HIGH) {
      Serial.println("released.");
    } else {
      Serial.println("pressed.");
    }
  }
}

void loopButtons() {
  uint8_t readValue = digitalRead(STOP_BUTTON_PIN);
  stopKey.cycle(readValue);
  if (stopKey.getAndResetEdgeFlag() && stopKey.getDebouncedValue() == HIGH) {
    if (displayManager.getSystemOn()) {
      RR32Can::RR32Can.SendSystemStop();
    } else {
      RR32Can::RR32Can.SendSystemGo();
    }
  }

  readValue = digitalRead(SHIFT_BUTTON_PIN);
  shiftKey.cycle(readValue);

  checkAndPrint(fKeys[0], "F0", F0_BUTTON_PIN);
  checkAndPrint(fKeys[1], "F1", F1_BUTTON_PIN);
  checkAndPrint(fKeys[2], "F2", F2_BUTTON_PIN);
  checkAndPrint(fKeys[3], "F3", F3_BUTTON_PIN);
  // checkAndPrint(fKeys[4], "F4", F4_BUTTON_PIN);
}

void setEngineVelocity(RR32Can::Velocity_t velocity) {
  if (displayMode == DisplayMode::ENGINE) {
    if (velocity < 0) {
      velocity = 0;
    }
    if (velocity >= RR32Can::kMaxEngineVelocity) {
      velocity = RR32Can::kMaxEngineVelocity;
    }
    forceEncoderPosition(velocity);
  }
}

#endif

} /* namespace EngineControl */

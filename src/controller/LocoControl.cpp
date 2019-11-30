#include "controller/LocoControl.h"
#include <controller/MasterControl.h>

#include "RR32Can/RR32Can.h"
#include "view/DisplayManager.h"
#include "view/UIAssets.h"

namespace controller {

void LocoControl::begin() { loco.reset(); }

void LocoControl::loop(model::InputState& inputState,
                       MasterControl& masterControl) {
  // Check Encoder button
  if (inputState.isEncoderRisingEdge()) {
    if (inputState.isShiftPressed()) {
      // switch to locolist on shift+encoder
      masterControl.enterLocoList();
    } else {
      // Encoder pressed
      if (loco.isFullDetailsKnown()) {
        // Send emergency stop for engine
        RR32Can::RR32Can.SendEmergencyStop(loco);
        // Send direction reverse for engine
        RR32Can::RR32Can.SendEngineDirection(
            loco, switchDirection(loco.getDirection()));
      }
    }
  }

  if (loco.isFullDetailsKnown()) {
    // Check function keys
    model::InputState::Key_t* functionKeys = inputState.getFunctionKeys();

    for (uint8_t i = 0; i < NUM_FBUTTONS; ++i) {
      if (functionKeys[i].getAndResetRisingEdge()) {
        // Always get and reset the edge
        uint8_t functionNum = i;
        if (i > 0 && inputState.isShiftPressed()) {
          // Note: F0 is not shifted
          functionNum += NUM_FBUTTONS - 1;
        }
        bool functionStatus = loco.getFunction(functionNum);
        RR32Can::RR32Can.SendEngineFunction(loco, functionNum, !functionStatus);
      }
    }

    // Check encoder
    checkEncoder(inputState);
  }
}

void LocoControl::checkEncoder(model::InputState& inputState) {
  model::InputState::EncoderPosition_t encoderPosition =
      inputState.encoder.getPosition();

  // See if there was an effective change to the reported encoder position
  if (inputState.lastEncoderState.position != encoderPosition) {
    inputState.lastEncoderState.position = encoderPosition;

    /* Limit newPosition to 0..kMaxEngineVelocity and reset the encoder
     appropriately, if necessary. */
    if (encoderPosition < 0) {
      encoderPosition = 0;
      inputState.loadEncoderPosition(encoderPosition);
    } else if (encoderPosition > RR32Can::kMaxEngineVelocity) {
      encoderPosition = RR32Can::kMaxEngineVelocity;
      inputState.loadEncoderPosition(encoderPosition);
    } else {
      // Encoder moved in bounds

      // evaluate acceleration...
      RotaryEncoder::Direction currentDirection =
          inputState.encoder.getDirection();
      if (currentDirection == inputState.lastEncoderState.direction &&
          currentDirection != RotaryEncoder::Direction::NOROTATION &&
          inputState.lastEncoderState.direction !=
              RotaryEncoder::Direction::NOROTATION) {
        // ... but only of the direction of rotation matched and there
        // actually was a previous rotation.
        unsigned long deltat = inputState.encoder.getMillisBetweenRotations();

        // at 500ms, there should be no acceleration.
        constexpr const unsigned long kAccelerationLongCutoffMillis = 500;
        if (deltat < kAccelerationLongCutoffMillis) {
          constexpr const unsigned long kAccelerationShortCutffMillis =
              4;  // 50/12
          if (deltat < kAccelerationShortCutffMillis) {
            // limit to maximum acceleration
            deltat = kAccelerationShortCutffMillis;
          }

          constexpr static const float m = -0.16;
          constexpr static const float c = 84.03;

          float ticksActual_float = m * deltat + c;
          // Round by adding 1
          // Then again remove 1 to determine the actual delta to the encoder
          // value, as the encoder already ticked by 1 tick in the correct
          // direction Thus, just cast to integer.
          long deltaTicks = (long)ticksActual_float;

          // Adjust sign: Needs to be inverted for counterclockwise operation
          if (currentDirection == RotaryEncoder::Direction::COUNTERCLOCKWISE) {
            deltaTicks = -(deltaTicks);
          }

          long newEncoderPosition = encoderPosition + deltaTicks;
          if (newEncoderPosition > RR32Can::kMaxEngineVelocity) {
            newEncoderPosition = RR32Can::kMaxEngineVelocity;
          } else if (newEncoderPosition < 0) {
            newEncoderPosition = 0;
          }

          encoderPosition = newEncoderPosition;
          inputState.loadEncoderPosition(newEncoderPosition);
        }
      } else {
        inputState.lastEncoderState.direction = currentDirection;
      }

      // Update the engine velocity

      if (loco.isFullDetailsKnown()) {
        RR32Can::Locomotive::Velocity_t oldVelocity = loco.getVelocity();
        if (encoderPosition != oldVelocity) {
          RR32Can::RR32Can.SendEngineVelocity(loco, encoderPosition);
        }
      }
    }
  }
}

void LocoControl::reset() { loco.reset(); }

bool LocoControl::setLocoInfo(const RR32Can::LocomotiveShortInfo& locoInfo) {
  if (strncmp(loco.getName(), locoInfo.getName(), RR32Can::kEngineNameLength) ==
      0) {
    return false;
  } else {
    loco.reset();
    loco.setName(locoInfo.getName());
    return true;
  }
}

void LocoControl::updateDisplayOnce(view::DisplayManager& displayManager) {
  {
    constexpr static const uint8_t nameLengthLimit =
        (STRING_CHAR_LENGTH > RR32Can::kEngineNameLength
             ? STRING_CHAR_LENGTH
             : RR32Can::kEngineNameLength);

    strncpy(displayManager.getWritableBuffer(0), loco.getName(),
            nameLengthLimit);
    displayManager.disableCursor();
  }

  if (loco.isFullDetailsKnown()) {
    char* buf = displayManager.getWritableBuffer(1);

    const char* protocolString = loco.getProtocolString();
    RR32Can::Locomotive::Address_t address = loco.getAddress();

    snprintf(buf, STRING_CHAR_LENGTH, "%s %i", protocolString, address);

  } else {
    strncpy(displayManager.getWritableBuffer(1), view::kFetching,
            STRING_CHAR_LENGTH);
  }
}

void LocoControl::updateDisplayLoop(view::DisplayManager& displayManager) {
  if (loco.isFullDetailsKnown()) {
    displayManager.setDirection(loco.getDirection());
    displayManager.setSpeedValue(
        map(loco.getVelocity(), 0, RR32Can::kMaxEngineVelocity, 0, 100));
    displayManager.setFunctionBits(loco.getFunctionBits());
  } else {
    displayManager.setDirection(RR32Can::EngineDirection::UNKNOWN);
    displayManager.setSpeedValue(0);
    displayManager.setFunctionBits(0u);
  }
}

void LocoControl::setReceivedVelocity(RR32Can::Velocity_t velocity,
                                      MasterControl& uiControl) {
  loco.setVelocity(velocity);
  if (uiControl.getUIMode() == MasterControl::UIMode::LOCOCONTROL) {
    model::InputState& inputState = uiControl.getInputState();
    if (velocity != inputState.encoder.getPosition()) {
      inputState.encoder.setPosition(velocity);
    }
  }
}

void LocoControl::requestLocoFile() {
  locoDataConsumer.setEngine(&loco);
  RR32Can::RR32Can.RequestEngine(loco, locoDataConsumer);
}

void LocoControl::requestLocoData() {
  RR32Can::RR32Can.RequestEngineVelocity(loco);
  RR32Can::RR32Can.RequestEngineDirection(loco);
  RR32Can::RR32Can.RequestEngineAllFunctions(loco);
}

}  // namespace controller

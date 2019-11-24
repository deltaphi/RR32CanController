#include "controller/TurnoutMenu.h"

#include "controller/MasterControl.h"

namespace controller {

void TurnoutMenu::begin() { currentKey = TURNOUT_BUTTONS_OFFSET; }

void TurnoutMenu::loop(model::InputState& inputState,
                       MasterControl& masterControl,
                       model::TurnoutMap& turnoutMap) {
  if (inputState.isEncoderRisingEdge()) {
    if (inputState.isShiftPressed()) {
      masterControl.enterSettingsMenu();
    } else {
      // TODO: Store current mapping
    }
  } else {
    // On encoder rotation, change the current mapping
    if (inputState.isEncoderMoved()) {
      model::InputState::EncoderPosition_t encoderNewPosition =
          inputState.encoder.getPosition();

      if (encoderNewPosition <
          static_cast<model::InputState::EncoderPosition_t>(
              RR32Can::kTurnoutAddressMin)) {
        // ran into lower limit
        inputState.loadEncoderPosition(RR32Can::kTurnoutAddressMin);
      } else if (encoderNewPosition >
                 static_cast<model::InputState::EncoderPosition_t>(
                     RR32Can::kTurnoutAddressMax)) {
        // ran into upper limit
        inputState.loadEncoderPosition(RR32Can::kTurnoutAddressMax);
      } else {
        // use the actual value
        model::TurnoutLookupResult turnoutLookup =
            turnoutMap.lookupTurnout(currentKey);
        turnoutLookup.address = encoderNewPosition;
        turnoutMap.setLookupTurnout(currentKey, turnoutLookup);
      }
      displayUpdateNeeded = true;
      inputState.consumeEncoderPosition();
    }

    // On turnout button press, map another turnout.
    model::InputState::Key_t* turnoutKeys = inputState.getTurnoutKeys();
    for (int i = 0; i < TURNOUT_BUTTONS_COUNT; ++i) {
      if (turnoutKeys[i].getAndResetRisingEdge()) {
        // Button was released, select the key.
        currentKey = i;
        inputState.loadEncoderPosition(
            turnoutMap.lookupTurnout(currentKey).address);
        displayUpdateNeeded = true;
        break;
      }
    }
  }
}

void TurnoutMenu::updateDisplay(view::DisplayManager& displayManager,
                                const model::TurnoutMap& turnoutMap) {
  if (displayUpdateNeeded) {
    snprintf(displayManager.getWritableBuffer(0), STRING_DATATYPE_LENGTH,
             "Button: %i (%s)", currentKey, "R/G");
    RR32Can::TurnoutAddress_t address =
        turnoutMap.lookupTurnout(currentKey).address;
    snprintf(displayManager.getWritableBuffer(1), STRING_DATATYPE_LENGTH,
             "Turnout: %i", address);

    displayUpdateNeeded = false;
  }
}

}  // namespace controller

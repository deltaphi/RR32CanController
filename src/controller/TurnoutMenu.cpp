#include "controller/TurnoutMenu.h"

#include "controller/MasterControl.h"

namespace controller {

void TurnoutMenu::begin() { currentKey = TURNOUT_BUTTONS_OFFSET; }

void TurnoutMenu::loadCurrentKey(
    model::InputState& inputState, model::TurnoutMap& turnoutMap,
    const model::ActionListDB::DB_t& actionListDb) {
  currentResult = turnoutMap.lookupTurnout(currentKey);
  inputState.loadEncoderPosition(
      RR32Can::HumanTurnoutAddress(currentResult.address).value());
  updateEncoderLimits(actionListDb);
  displayUpdateNeeded = true;
}

void TurnoutMenu::updateEncoderLimits(
    const model::ActionListDB::DB_t& actionListDb) {
  switch (currentResult.mode) {
    case model::TurnoutAddressMode::SingleTurnout:
      limiter.setMin(
          RR32Can::HumanTurnoutAddress(RR32Can::kTurnoutAddressMin).value());
      limiter.setMax(
          RR32Can::HumanTurnoutAddress(RR32Can::kTurnoutAddressMax).value());
      break;

    case model::TurnoutAddressMode::MultiTurnout:
      limiter.setMin(1);
      limiter.setMax(actionListDb.size());
      break;
  }
}

void TurnoutMenu::loop(model::InputState& inputState,
                       MasterControl& masterControl,
                       model::TurnoutMap& turnoutMap,
                       const model::ActionListDB::DB_t& actionListDb) {
  if (inputState.isEncoderRisingEdge()) {
    if (inputState.isShiftPressed()) {
      // Persisently save current mapping and exit the menu.
      turnoutMap.store();
      masterControl.enterSettingsMenu();
    } else {
      // Store current mapping in volatile storage.
      turnoutMap.setLookupTurnout(currentKey, currentResult);
    }
  } else {
    model::InputState::Key_t* functionKey = inputState.getFunctionKeys();
    if (functionKey[0].getAndResetRisingEdge()) {
      currentResult.mode = SwitchMode(currentResult.mode);
      updateEncoderLimits(actionListDb);
      displayUpdateNeeded = true;
    }

    // On encoder rotation, change the current mapping
    if (inputState.isEncoderMoved()) {
      model::InputState::EncoderPosition_t encoderNewPosition =
          inputState.encoder.getPosition();

      model::InputState::EncoderPosition_t limitedPosition =
          limiter.limitedValue(encoderNewPosition);

      if (limitedPosition != encoderNewPosition) {
        // Hit a limit
        inputState.loadEncoderPosition(limitedPosition);
      } else {
        // use the actual value
        currentResult.address =
            RR32Can::HumanTurnoutAddress(encoderNewPosition);
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
        loadCurrentKey(inputState, turnoutMap, actionListDb);
        break;
      }
    }
  }
}

void TurnoutMenu::updateDisplay(application::model::DisplayModel& displayManager,
                                const model::TurnoutMap& turnoutMap) {
  if (displayUpdateNeeded) {
    snprintf(displayManager.getWritableBuffer(0), STRING_DATATYPE_LENGTH,
             "Button: %i (%s)", currentKey, "R/G");
    RR32Can::HumanTurnoutAddress address = currentResult.address;

    snprintf(displayManager.getWritableBuffer(1), STRING_DATATYPE_LENGTH,
             "%s: %i",
             (currentResult.mode == model::TurnoutAddressMode::SingleTurnout
                  ? "Turnout"
                  : "ActionL"),
             address.value());

    displayUpdateNeeded = false;
  }
}

}  // namespace controller

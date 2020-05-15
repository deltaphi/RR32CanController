#include "controller/TurnoutMenu.h"

#include "controller/MasterControl.h"

namespace controller {

void TurnoutMenu::begin(
    application::controller::TurnoutMapStorageCbk& turnoutMapStorageCbk) {
  currentKey = TURNOUT_BUTTONS_OFFSET;
  this->turnoutMapStorageCbk = &turnoutMapStorageCbk;
}

void TurnoutMenu::loadCurrentKey(
    application::model::InputState& inputState,
    application::model::TurnoutMap& turnoutMap,
    const application::model::ActionListModel::DB_t& actionListDb) {
  currentResult = turnoutMap.lookupTurnout(currentKey);
  inputState.loadEncoderPosition(
      RR32Can::HumanTurnoutAddress(currentResult.address).value());
  updateEncoderLimits(actionListDb);
  displayUpdateNeeded = true;
}

void TurnoutMenu::updateEncoderLimits(
    const application::model::ActionListModel::DB_t& actionListDb) {
  switch (currentResult.mode) {
    case application::model::TurnoutAddressMode::SingleTurnout:
      limiter.setMin(
          RR32Can::HumanTurnoutAddress(RR32Can::kTurnoutAddressMin).value());
      limiter.setMax(
          RR32Can::HumanTurnoutAddress(RR32Can::kTurnoutAddressMax).value());
      break;

    case application::model::TurnoutAddressMode::MultiTurnout:
      limiter.setMin(1);
      limiter.setMax(actionListDb.size());
      break;
  }
}

void TurnoutMenu::loop(
    application::model::InputState& inputState, MasterControl& masterControl,
    application::model::TurnoutMap& turnoutMap,
    const application::model::ActionListModel::DB_t& actionListDb) {
  if (inputState.isEncoderRisingEdge()) {
    if (inputState.isShiftPressed()) {
      // Persisently save current mapping and exit the menu.
      turnoutMapStorageCbk->store(turnoutMap);
      masterControl.enterSettingsMenu();
    } else {
      // Store current mapping in volatile storage.
      turnoutMap.setLookupTurnout(currentKey, currentResult);
    }
  } else {
    application::model::InputState::Key_t* functionKey =
        inputState.getFunctionKeys();
    if (functionKey[0].getAndResetRisingEdge()) {
      currentResult.mode = SwitchMode(currentResult.mode);
      updateEncoderLimits(actionListDb);
      displayUpdateNeeded = true;
    }

    // On encoder rotation, change the current mapping
    if (inputState.isEncoderMoved()) {
      application::model::InputState::EncoderPosition_t encoderNewPosition =
          inputState.encoder.getPosition();

      application::model::InputState::EncoderPosition_t limitedPosition =
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
    application::model::InputState::Key_t* turnoutKeys =
        inputState.getTurnoutKeys();
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

void TurnoutMenu::updateDisplay(
    application::model::DisplayModel& displayManager,
    const application::model::TurnoutMap& turnoutMap) {
  if (displayUpdateNeeded) {
    snprintf(displayManager.getWritableBuffer(0), STRING_DATATYPE_LENGTH,
             "Button: %i (%s)", currentKey, "R/G");
    RR32Can::HumanTurnoutAddress address = currentResult.address;

    snprintf(displayManager.getWritableBuffer(1), STRING_DATATYPE_LENGTH,
             "%s: %i",
             (currentResult.mode ==
                      application::model::TurnoutAddressMode::SingleTurnout
                  ? "Turnout"
                  : "ActionL"),
             address.value());

    displayUpdateNeeded = false;
  }
}

}  // namespace controller

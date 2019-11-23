#include "controller/Turnout.h"

#include "config.h"
#include "model/InputState.h"

#include "RR32Can/Constants.h"
#include "RR32Can/RR32Can.h"
#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"
#include "RR32Can/messages/TurnoutPacket.h"

namespace controller {

void Turnout::begin() {}

void Turnout::loop(model::InputState& inputState) {
  model::InputState::Key_t* keys = inputState.getTurnoutKeys();

  for (int i = 0; i < TURNOUT_BUTTONS_COUNT; ++i) {
    if (keys[i].getAndResetEdgeFlag()) {
      handleButton(i, keys[i].getDebouncedValue());
    }
  }

  actionListProcessor.loop();
}

void Turnout::handleMultiturnout(TurnoutControl::TurnoutLookupResult result,
                                 RR32Can::TurnoutDirection requestedDirection) {
  --result.address;  // Simple mapping to index into actionLists
  constexpr uint8_t actionListEndIndex =
      (TurnoutControl::NumActionLists /
       ((uint8_t)2U));  // Index one past the end of half the action lists
  if (result.address >= actionListEndIndex) {
    Serial.print("Requested Action List ");
    Serial.print(result.address, DEC);
    Serial.println(" is out of range.");
    return;
  }

  if (requestedDirection == RR32Can::TurnoutDirection::RED) {
    // Add offset into the green lists
    result.address += actionListEndIndex;
  }

  Serial.print("Starting Action List ");
  Serial.println(result.address, DEC);

#if (LOG_ACTIONLIST == STD_ON)
  Serial.print("Requesting action list ");
  Serial.println(result.address, DEC);
#endif
  if (!actionListProcessor.requestActionList(result.address)) {
    Serial.println(
        "An action list is already active. Ignoring request for an additional "
        "one.");
  }
}

/**
 * \brief Called when the debouncing algorithm has determined that a button has
 *        changed position.
 */
void Turnout::handleButton(uint8_t buttonIndex, uint8_t buttonState) {
  // There was an edge here, send out a message
  TurnoutControl::TurnoutLookupResult turnoutIndex =
      TurnoutControl::lookupTurnout(buttonIndex);

#if (LOG_BUTTON_PRESS == STD_ON)
  Serial.print(F("Button "));
  Serial.print(buttonIndex);
  if (buttonState == HIGH) {
    // Someone pressed this button
    Serial.println(F(" was pressed."));
  } else {
    // Someone released this button
    Serial.println(F(" was released."));
  }
#endif

  switch (turnoutIndex.mode) {
    case TurnoutControl::TurnoutAddressMode::SingleTurnout: {
      // Single turnout - send out a packet right away.
      RR32Can::TurnoutDirection direction =
          buttonIndex % 2 == 0 ? RR32Can::TurnoutDirection::RED
                               : RR32Can::TurnoutDirection::GREEN;
      direction =
          (direction == RR32Can::TurnoutDirection::RED
               ? RR32Can::TurnoutDirection::GREEN
               : RR32Can::TurnoutDirection::RED);  // invert, as my wires
                                                   // are connected in the
                                                   // opposite order

      RR32Can::RR32Can.SendAccessoryPacket(turnoutIndex.address, direction,
                                           (buttonState == HIGH ? 1 : 0));
      break;
    }
    case TurnoutControl::TurnoutAddressMode::MultiTurnout: {
      // Only start multi-turnout actions on button press, not release.
      if (buttonState == HIGH) {
        handleMultiturnout(
            turnoutIndex,
            (buttonIndex % 2 == 0 ? RR32Can::TurnoutDirection::RED
                                  : RR32Can::TurnoutDirection::GREEN));
      }
      break;
    }
  }
}

}  // namespace controller

#include "application/controller/Turnout.h"

#include "config.h"

#include "RR32Can/Constants.h"
#include "RR32Can/RR32Can.h"
#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"
#include "RR32Can/messages/TurnoutPacket.h"

#include "application/model/TurnoutTypes.h"

namespace application {
namespace controller {

void Turnout::begin(application::controller::ActionlistStorageCbk& scbk,
                    application::controller::TurnoutMapStorageCbk& tmcbk) {
  turnoutMap.begin();
  tmcbk.load(turnoutMap);
  scbk.load(actionList.getDb());
  actionListProcessor.begin(actionList);
}

void Turnout::loop(application::model::InputState& inputState) {
  application::model::InputState::Key_t* keys = inputState.getTurnoutKeys();

  for (int i = 0; i < TURNOUT_BUTTONS_COUNT; ++i) {
    if (keys[i].getAndResetEdgeFlag()) {
      handleButton(i, keys[i].getDebouncedValue());
    }
  }

  actionListProcessor.loop();
}

void Turnout::handleMultiturnout(application::model::TurnoutLookupResult result,
                                 RR32Can::TurnoutDirection requestedDirection) {
  RR32Can::TurnoutAddressBase::value_type actionListAddr = result.address.value();

  if (requestedDirection == RR32Can::TurnoutDirection::RED) {
    // Add offset into the green lists
    ++actionListAddr;
  }

#if (LOG_ACTIONLIST == STD_ON)
  printf("Requesting action list %i.\n", actionListAddr);
#endif
  if (!actionListProcessor.requestActionList(actionListAddr)) {
    printf(
        "An action list is already active. Ignoring request for an additional "
        "one.\n");
  }
}

/**
 * \brief Called when the debouncing algorithm has determined that a button has
 *        changed position.
 */
void Turnout::handleButton(uint8_t buttonIndex, uint8_t buttonState) {
  // There was an edge here, send out a message
  application::model::TurnoutLookupResult turnoutIndex = turnoutMap.lookupTurnout(buttonIndex);

#if (LOG_BUTTON_PRESS == STD_ON)
  printf("Button %i was %s.\n", buttonIndex, (buttonState == HIGH ? "pressed" : "released"));
#endif

  switch (turnoutIndex.mode) {
    case application::model::TurnoutAddressMode::SingleTurnout: {
      // Single turnout - send out a packet right away.
      RR32Can::TurnoutDirection direction =
          buttonIndex % 2 == 0 ? RR32Can::TurnoutDirection::RED : RR32Can::TurnoutDirection::GREEN;
      direction = (direction == RR32Can::TurnoutDirection::RED
                       ? RR32Can::TurnoutDirection::GREEN
                       : RR32Can::TurnoutDirection::RED);  // invert, as my wires
                                                           // are connected in the
                                                           // opposite order

      RR32Can::RR32Can.SendAccessoryPacket(turnoutIndex.address, direction,
                                           (buttonState == HIGH ? 1 : 0));
      break;
    }
    case application::model::TurnoutAddressMode::MultiTurnout: {
      // Only start multi-turnout actions on button press, not release.
      if (buttonState == HIGH) {
        handleMultiturnout(turnoutIndex, (buttonIndex % 2 == 0 ? RR32Can::TurnoutDirection::RED
                                                               : RR32Can::TurnoutDirection::GREEN));
      }
      break;
    }
  }
}

}  // namespace controller
}  // namespace application

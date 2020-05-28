#include "application/controller/ActionListControl.h"

#include "RR32Can/RR32Can.h"

#include "Arduino.h"

#include <cstdio>

namespace application {
namespace controller {

void ActionListControl::loop() {
  if (!hasActiveAction()) {
    return;  // No action list active.
  } else {
    if (this->currentAction == (*currentActionList).end()) {
      // An action list is active but has not been started.
      lastActionTime = micros();

      this->currentAction = (*currentActionList).begin();
      buttonPressed = false;
      performAction();
      // After starting, the second operation will be a button release, so we
      // don't have to update the state machine
    } else {
      // An action list is active and has been started.
      // Determine wheter we need to perform the next action already
      unsigned long currentSleepDuration = micros() - lastActionTime;
      if (requestedSleepDuration <= currentSleepDuration) {
        // Action is required
        performAction();
        lastActionTime = micros();
        if (!buttonPressed) {
          // Advance the actionIndex
          this->currentAction++;
          if (this->currentAction == (*currentActionList).end()) {
            // we reached the end of the action list.
            setInactive();
          }
        }
      }
    }
  }
}

void ActionListControl::performAction() {
  application::model::TurnoutAction& action = *(this->currentAction);
#if (LOG_ACTIONLIST == STD_ON)
  printf("Action:");
  print(action);
#endif

  if (!buttonPressed) {
    // Generate a power on message.
    RR32Can::RR32Can.SendAccessoryPacket(action.address, action.direction, 1);

    buttonPressed = true;
  } else {
    // Generate a button release
    RR32Can::RR32Can.SendAccessoryPacket(action.address, action.direction, 0);

    buttonPressed = false;
  }
}

bool ActionListControl::requestActionList(uint8_t actionListIndex) {
  if (hasActiveAction()) {
    return false;
  } else {
    this->currentActionList = db->begin();
    std::advance(this->currentActionList, actionListIndex);
    this->currentAction = (*currentActionList).end();
    this->buttonPressed = false;
    return true;
  }
}

}  // namespace controller
}  // namespace application

#include "ActionListProcessor.h"
#include "config.h"

#include "RR32Can/handler.h"

namespace TurnoutControl {

void ActionListProcessor::loop() {
  if (!hasActiveAction()) {
    return;  // No action list active.
  } else {
    if (actionIndex == kActionListNotStarted) {
      // An action list is active but has not been started.
      lastActionTime = micros();

      actionIndex = 0;
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
          ++actionIndex;
          if (actionIndex >= NumActions) {
            // we reached the end of the action list.
            setInactive();
          }
        }
      }
    }
  }
}

void ActionListProcessor::performAction() {
#if (LOG_ACTIONLIST == STD_ON)
  Serial.print("Performing action for index ");
  Serial.print(actionIndex, DEC);
#endif
  TurnoutAction* action = &(actionLists[actionListIndex][actionIndex]);
#if (LOG_ACTIONLIST == STD_ON)
  Serial.print(". Action pointer is 0x");
  Serial.print((uint32_t)action, HEX);
  Serial.print(", Address is ");
  Serial.print(action->address);
  Serial.print(", Direction is ");
  Serial.println((uint8_t)action->direction);
#endif

  if (!buttonPressed) {
    // Generate a power on message.
    // Adjust from human to technical adressing
    RR32Can::SendAccessoryPacket(action->address - 1, action->direction, 1);

    buttonPressed = true;
  } else {
    // Generate a button release
    // Adjust from human to technical adressing
    RR32Can::SendAccessoryPacket(action->address - 1, action->direction, 0);

    buttonPressed = false;
  }
}

bool ActionListProcessor::requestActionList(uint8_t actionListIndex) {
  if (hasActiveAction()) {
    return false;
  } else {
    this->actionListIndex = actionListIndex;
    this->actionIndex = kActionListNotStarted;
    this->buttonPressed = false;
    return true;
  }
}

} /* namespace TurnoutControl */
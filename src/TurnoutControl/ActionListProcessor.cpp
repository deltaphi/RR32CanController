#include "ActionListProcessor.h"
#include "config.h"

#include "RR32Can/RR32Can.h"

namespace TurnoutControl {

void ActionListProcessor::loop() {
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

void ActionListProcessor::performAction() {
#if (LOG_ACTIONLIST == STD_ON)
  Serial.print("Performing action for index ");
  Serial.print(actionIndex, DEC);
#endif
  model::TurnoutAction& action = *(this->currentAction);
#if (LOG_ACTIONLIST == STD_ON)
  Serial.print(". Action pointer is 0x");
  Serial.print((uint32_t)action, HEX);
  Serial.print(", Address is ");
  Serial.print(RR32Can::HumanTurnoutAddress(action->address).value());
  Serial.print(", Direction is ");
  Serial.println(
      RR32Can::TurnoutDirectionToIntegral<uint8_t>(action->direction));
#endif

  if (!buttonPressed) {
    // Generate a power on message.
    RR32Can::RR32Can.SendAccessoryPacket(action.address,
                                         action.direction, 1);

    buttonPressed = true;
  } else {
    // Generate a button release
    RR32Can::RR32Can.SendAccessoryPacket(action.address,
                                         action.direction, 0);

    buttonPressed = false;
  }
}

bool ActionListProcessor::requestActionList(uint8_t actionListIndex) {
  if (hasActiveAction()) {
    return false;
  } else {
    this->currentActionList = db.begin();
    std::advance(this->currentActionList, actionListIndex);
    this->currentAction = (*currentActionList).end();
    this->buttonPressed = false;
    return true;
  }
}

void ActionListProcessor::printActionList(
    model::ActionListDB::Index_t index) const {
  model::ActionListDB::DB_t::const_iterator dbIt = db.begin();
  std::advance(dbIt, index);

  for (const model::TurnoutAction& action : *dbIt) {
    print(action);
  }
}

void ActionListProcessor::printActionLists() const {
  printf("Printing %i Action Lists:\n", db.size());

  model::ActionListDB::DB_t::const_iterator dbIt = db.begin();
  int dbIdx = 0;
  while (dbIt != db.end()) {
    printf("Action List %i:\n", dbIdx);

    for (const model::TurnoutAction& action : *dbIt) {
      print(action);
    }

    ++dbIt;
    ++dbIdx;
  }

  printf("done.\n");
}

} /* namespace TurnoutControl */

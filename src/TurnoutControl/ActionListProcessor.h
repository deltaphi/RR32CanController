#ifndef __ACTION_LIST_PROCESSOR_H__
#define __ACTION_LIST_PROCESSOR_H__

#include "RR32Can/Types.h"

#include "model/TurnoutTypes.h"

namespace TurnoutControl {

class ActionListProcessor {
 public:
  typedef struct {
    uint8_t address;
    RR32Can::TurnoutDirection direction;
  } Action;

  bool hasActiveAction() const {
    return actionListIndex != kActionListNotStarted;
  }

  bool requestActionList(uint8_t actionListIndex);

  void loop();

  uint8_t getNumActionLists() const {
    return model::NumActionLists;
  }

 private:
  void performAction();

  void setInactive() {
    actionListIndex = kActionListNotStarted;
    actionIndex = kActionListNotStarted;
    buttonPressed = false;
  }

  constexpr static uint8_t kActionListNotStarted = 0xFF;

  /**
   * The currently active action list, if any.
   */
  uint8_t actionListIndex = kActionListNotStarted;

  /**
   * The currently active action out of the above action list.
   * nullptr denotes that no action list is actove or that processing of the
   * active list has not started.
   */
  uint8_t actionIndex = kActionListNotStarted;

  /**
   * Note whether the last action was a button press (true) or release (false).
   */
  bool buttonPressed = false;

  unsigned long lastActionTime;
  constexpr static unsigned long requestedSleepDuration =
      200000;  // Sleep for 200ms, i.e, 200'000us
};

} /* namespace TurnoutControl */

#endif

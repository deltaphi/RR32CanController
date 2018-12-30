#ifndef __ACTION_LIST_PROCESSOR_H__
#define __ACTION_LIST_PROCESSOR_H__

#include "TurnoutTypes.h"

// Forward declaration for this function
void sendTurnoutPacket(uint32_t turnoutAddress, TurnoutDirection direction,
                       uint8_t power);

class ActionListProcessor {
 public:
  typedef struct {
    uint8_t address;
    TurnoutDirection direction;
  } Action;

  bool hasActiveAction() const { return actionListIndex != kActionListNotStarted; }

  bool requestActionList(uint8_t actionListIndex);

  void loop();

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

#endif
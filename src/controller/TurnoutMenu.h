#ifndef __CONTROLLER__TURNOUTMENU_H__
#define __CONTROLLER__TURNOUTMENU_H__

#include <cstdint>

#include "model/InputState.h"
#include "view/DisplayManager.h"

namespace controller {

class MasterControl;

/*
 * \brief Class TurnoutMenu
 */
class TurnoutMenu {
 public:
  using TurnoutKeyIndex_t = uint8_t;

  void begin();
  void loop(model::InputState& inputState, MasterControl& masterControl);

  void updateDisplay(view::DisplayManager& displayManager);
  void updateDisplayOnce(view::DisplayManager& displayManager) {
    displayUpdateNeeded = true;
    updateDisplay(displayManager);
  }

 private:
  TurnoutKeyIndex_t currentKey;

  /// Whether an action was taken that requires an update to the display.
  bool displayUpdateNeeded;
};

}  // namespace controller

#endif  // __CONTROLLER__TURNOUTMENU_H__

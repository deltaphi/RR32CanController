#ifndef __CONTROLLER__TURNOUTMENU_H__
#define __CONTROLLER__TURNOUTMENU_H__

#include <cstdint>

#include "model/InputState.h"
#include "model/TurnoutMap.h"
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
  void loop(model::InputState& inputState, MasterControl& masterControl,
            model::TurnoutMap& turnoutMap);

  void updateDisplay(view::DisplayManager& displayManager,
                     const model::TurnoutMap& turnoutMap);
  void forceDisplayUpdate() { displayUpdateNeeded = true; }

 private:
  TurnoutKeyIndex_t currentKey;
  model::TurnoutLookupResult currentResult;

  void loadCurrentKey(model::InputState& inputState,
                      model::TurnoutMap& turnoutMap);

  /// Whether an action was taken that requires an update to the display.
  bool displayUpdateNeeded;
};

}  // namespace controller

#endif  // __CONTROLLER__TURNOUTMENU_H__

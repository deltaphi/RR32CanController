#ifndef __CONTROLLER__IDLE_H__
#define __CONTROLLER__IDLE_H__

#include "model/InputState.h"
#include "view/DisplayManager.h"

namespace controller {

class UIControl;

/*
 * \brief Class Idle
 */
class Idle {
 public:
  void begin(){};
  void loop(model::InputState& inputState, UIControl& uiControl);

  void updateDisplayOnce(view::DisplayManager& displayManager);

 private:
};

}  // namespace controller

#endif  // __CONTROLLER__IDLE_H__

#ifndef __CONTROLLER__IDLE_H__
#define __CONTROLLER__IDLE_H__

#include "application/model/InputState.h"
#include "application/model/DisplayModel.h"

namespace controller {

class MasterControl;

/*
 * \brief Class Idle
 */
class Idle {
 public:
  void begin(){};
  void loop(application::model::InputState& inputState, MasterControl& uiControl);

  void updateDisplayOnce(application::model::DisplayModel& displayManager);

 private:
};

}  // namespace controller

#endif  // __CONTROLLER__IDLE_H__

#ifndef __APPLICATION__CONTROLLER__IDLE_H__
#define __APPLICATION__CONTROLLER__IDLE_H__

#include "application/model/InputState.h"
#include "application/model/DisplayModel.h"

namespace application {
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
}  // namespace application

#endif  // __APPLICATION__CONTROLLER__IDLE_H__

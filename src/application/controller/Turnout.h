#ifndef __APPLICATION__CONTROLLER__TURNOUT_H__
#define __APPLICATION__CONTROLLER__TURNOUT_H__

#include "application/controller/ActionListControl.h"
#include "application/controller/ActionlistStorageCbk.h"
#include "application/controller/TurnoutMapStorageCbk.h"
#include "application/model/InputState.h"
#include "application/model/TurnoutMap.h"
#include "application/model/TurnoutTypes.h"

namespace application {
namespace controller {

/*
 * \brief Class Turnout
 */
class Turnout {
 public:
  void begin(
      application::controller::ActionlistStorageCbk& scbk,
      application::controller::TurnoutMapStorageCbk& turnoutMapStorageCallback);
  void loop(application::model::InputState& inputState);

  application::model::TurnoutMap& getTurnoutMap() { return turnoutMap; };

  application::model::ActionListModel& getActionListModel() {
    return actionList;
  }

  application::controller::ActionListControl& getActionListProcessor() {
    return actionListProcessor;
  }

 private:
  void handleButton(uint8_t buttonIndex, uint8_t buttonState);
  void handleMultiturnout(application::model::TurnoutLookupResult result,
                          RR32Can::TurnoutDirection requestedDirection);

  application::model::TurnoutMap turnoutMap;

  application::model::ActionListModel actionList;
  application::controller::ActionListControl actionListProcessor;
};

}  // namespace controller
}  // namespace application

#endif  // __APPLICATION__CONTROLLER__TURNOUT_H__

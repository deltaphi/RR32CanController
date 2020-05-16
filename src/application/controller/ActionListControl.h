#ifndef __APPLICATION__CONTROLLER__ACTIONLISTCONTROL_H__
#define __APPLICATION__CONTROLLER__ACTIONLISTCONTROL_H__

#include "RR32Can/Types.h"

#include "application/controller/ActionlistStorageCbk.h"
#include "application/model/ActionListModel.h"

namespace application {
namespace controller {

/*
 * \brief Class ActionListControl
 */
class ActionListControl {
 public:
  void begin(application::model::ActionListModel& actionList) {
    this->db = &(actionList.getDb());
    setInactive();
  }

  typedef struct {
    uint8_t address;
    RR32Can::TurnoutDirection direction;
  } Action;

  bool hasActiveAction() const { return currentActionList != db->end(); }

  bool requestActionList(uint8_t actionListIndex);

  void loop();

 private:
  application::model::ActionListModel::DB_t* db;

  void performAction();

  void setInactive() {
    currentActionList = db->end();
    currentAction = application::model::ActionListModel::ActionList_t::iterator();
    buttonPressed = false;
  }

  /**
   * The currently active action list, if any.
   */
  application::model::ActionListModel::DB_t::iterator currentActionList;

  /**
   * The currently active action out of the above action list.
   * nullptr denotes that no action list is actove or that processing of the
   * active list has not started.
   */
  application::model::ActionListModel::ActionList_t::iterator currentAction;

  /**
   * Note whether the last action was a button press (true) or release (false).
   */
  bool buttonPressed = false;

  unsigned long lastActionTime;
  constexpr static unsigned long requestedSleepDuration =
      200000;  // Sleep for 200ms, i.e, 200'000us
};

}  // namespace controller
}  // namespace application

#endif  // __APPLICATION__CONTROLLER__ACTIONLISTCONTROL_H__

#ifndef __CONTROLLER__TURNOUTMENU_H__
#define __CONTROLLER__TURNOUTMENU_H__

#include <cstdint>

#include "application/controller/TurnoutMapStorageCbk.h"
#include "application/model/ActionListModel.h"
#include "application/model/DisplayModel.h"
#include "application/model/InputState.h"
#include "application/model/TurnoutMap.h"
#include "application/controller/EncoderLimiter.h"


namespace controller {

class MasterControl;

/*
 * \brief Class TurnoutMenu
 */
class TurnoutMenu {
 public:
  using TurnoutKeyIndex_t = uint8_t;

  void begin(
      application::controller::TurnoutMapStorageCbk& turnoutMapStorageCbk);
  void loop(application::model::InputState& inputState,
            MasterControl& masterControl,
            application::model::TurnoutMap& turnoutMap,
            const application::model::ActionListModel::DB_t& actionListDb);

  void updateDisplay(application::model::DisplayModel& displayManager,
                     const application::model::TurnoutMap& turnoutMap);
  void forceDisplayUpdate() { displayUpdateNeeded = true; }

 private:
  TurnoutKeyIndex_t currentKey;
  application::model::TurnoutLookupResult currentResult;
  application::controller::TurnoutMapStorageCbk* turnoutMapStorageCbk;

  void loadCurrentKey(
      application::model::InputState& inputState,
      application::model::TurnoutMap& turnoutMap,
      const application::model::ActionListModel::DB_t& actionListDb);

  void updateEncoderLimits(
      const application::model::ActionListModel::DB_t& actionListDb);

  /// Whether an action was taken that requires an update to the display.
  bool displayUpdateNeeded;

  application::controller::EncoderLimiter limiter;
};

}  // namespace controller

#endif  // __CONTROLLER__TURNOUTMENU_H__

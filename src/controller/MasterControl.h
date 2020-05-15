#ifndef __CONTROLLER__UICONTROL_H__
#define __CONTROLLER__UICONTROL_H__

#include "application/model/DisplayModel.h"
#include "controller/Idle.h"
#include "controller/LocoControl.h"
#include "controller/LocoList.h"
#include "controller/SettingsMenu.h"
#include "controller/Turnout.h"
#include "controller/TurnoutMenu.h"


#include "RR32Can/StationCbk.h"

namespace controller {

/*
 * \brief Class UIControl
 */
class MasterControl : public RR32Can::StationCbk {
 public:
  enum class UIMode {
    IDLE = 0,
    LOCOCONTROL,
    LOCOLIST,
    LOCODOWNLOAD,
    SETTINGS,
    TURNOUTMAPPING
  };

  virtual ~MasterControl() = default;

  void begin(
      application::controller::SettingsStorageCbk& settingsCbk,
      application::controller::TurnoutMapStorageCbk& turnoutMapStorageCallback,
      application::controller::ActionlistStorageCbk& actionListCallback);

  void loop();

  // Use by other controllers?
  application::model::DisplayModel& getDisplayModel() { return displayModel; }

  // State transition functions
  void enterIdle();
  void enterLocoControl();
  void enterLocoList();
  void enterLocoDownload();
  void enterSettingsMenu();
  void enterTurnoutMenu();

  UIMode getUIMode() const { return uiMode; }


  application::model::InputState& getInputState() {
    return inputState;
  }


  RR32Can::Locomotive* getLoco(RR32Can::Locomotive::Uid_t uid) override;
  void setLocoVelocity(RR32Can::Locomotive::Uid_t uid,
                       RR32Can::Velocity_t velocity) override;
  void setLocoVelocity(RR32Can::Velocity_t velocity) override;

  void setSystemState(bool onOff) override { displayModel.setSystem(onOff); }

  const application::model::Settings& getUserSettings() const {
    return settingsMenu.getUserSettings();
  }

  application::model::ActionListModel& getActionListModel() {
    return turnoutControl.getActionListModel();
  }

  application::controller::ActionListControl& getActionListProcessor() {
    return turnoutControl.getActionListProcessor();
  }

 private:
  void checkStateTransition();
  void forwardLoop();
  void updateDisplayLoop();

  void loopStopKey();

  application::model::InputState inputState;

  application::model::DisplayModel displayModel;
  UIMode uiMode = UIMode::IDLE;

  Idle idleControl;
  LocoControl locoControl;
  LocoList locoList;

  Turnout turnoutControl;

  SettingsMenu settingsMenu;
  TurnoutMenu turnoutMenu;


  RR32Can::SystemState systemState = RR32Can::SystemState::UNKNOWN;
};

}  // namespace controller

#endif  // __CONTROLLER__UICONTROL_H__

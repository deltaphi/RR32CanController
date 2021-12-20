#ifndef __APPLICATION__CONTROLLER__UICONTROL_H__
#define __APPLICATION__CONTROLLER__UICONTROL_H__

#include "application/controller/Idle.h"
#include "application/controller/LocoControl.h"
#include "application/controller/LocoList.h"
#include "application/controller/SettingsMenu.h"
#include "application/controller/Turnout.h"
#include "application/controller/TurnoutMenu.h"
#include "application/model/DisplayModel.h"

#include "RR32Can/callback/EngineCbk.h"
#include "RR32Can/callback/SystemCbk.h"

namespace application {
namespace controller {

/*
 * \brief Class UIControl
 */
class MasterControl : public RR32Can::callback::EngineCbk, public RR32Can::callback::SystemCbk {
 public:
  enum class UIMode { IDLE = 0, LOCOCONTROL, LOCOLIST, LOCODOWNLOAD, SETTINGS, TURNOUTMAPPING };

  virtual ~MasterControl() = default;

  void begin(application::controller::SettingsStorageCbk& settingsCbk,
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

  application::model::InputState& getInputState() { return inputState; }

  void setLocoVelocity(RR32Can::Locomotive::Uid_t uid, RR32Can::Velocity_t velocity) override;
  void setLocoVelocity(RR32Can::Velocity_t velocity) override;

  void setSystemState(bool onOff, bool response) override { displayModel.setSystem(onOff); }

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

  RR32Can::Locomotive* getLoco(RR32Can::Locomotive::Uid_t uid);

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
}  // namespace application

#endif  // __APPLICATION__CONTROLLER__UICONTROL_H__

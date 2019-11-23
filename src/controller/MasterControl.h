#ifndef __CONTROLLER__UICONTROL_H__
#define __CONTROLLER__UICONTROL_H__

#include "controller/Idle.h"
#include "controller/Input.h"
#include "controller/LocoControl.h"
#include "controller/LocoList.h"
#include "controller/Turnout.h"
#include "view/DisplayManager.h"

#include "RR32Can/StationCbk.h"

namespace controller {

/*
 * \brief Class UIControl
 */
class MasterControl : public RR32Can::StationCbk {
 public:
  enum class UIMode { IDLE = 0, LOCOCONTROL, LOCOLIST, LOCODOWNLOAD };

  virtual ~MasterControl() = default;

  void begin();

  void loop();

  // Use by other controllers?
  view::DisplayManager& getDisplayManager() { return displayManager; }

  // State transition functions
  void enterIdle();
  void enterLocoControl();
  void enterLocoList();
  void enterLocoDownload();

  UIMode getUIMode() const { return uiMode; }

  model::InputState getInputState() { return input.getInputState(); }

  RR32Can::Locomotive* getLoco(RR32Can::Locomotive::Uid_t uid) override;
  void setLocoVelocity(RR32Can::Locomotive::Uid_t uid,
                       RR32Can::Velocity_t velocity) override;
  void setLocoVelocity(RR32Can::Velocity_t velocity) override;

  void setSystemState(bool onOff) override { displayManager.setSystem(onOff); }

 private:
  void checkStateTransition();
  void forwardLoop();
  void updateDisplayLoop();

  void loopStopKey();

  Input input;

  view::DisplayManager displayManager;
  UIMode uiMode = UIMode::IDLE;

  Idle idleControl;
  LocoControl locoControl;
  LocoList locoList;

  Turnout turnoutControl;

  RR32Can::SystemState systemState = RR32Can::SystemState::UNKNOWN;
};

}  // namespace controller

#endif  // __CONTROLLER__UICONTROL_H__

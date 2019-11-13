#include <controller/MasterControl.h>
#include <cstring>

#include "view/UIAssets.h"

#include "RR32Can/RR32Can.h"

namespace controller {

void MasterControl::begin() {
  systemState = RR32Can::SystemState::UNKNOWN;

  displayManager.begin();
  enterIdle();

  input.begin();
  locoControl.begin();
  locoList.begin();
}

void MasterControl::loop() {
  input.loop();

  // Always evaluate the STOP key with no SHIFT support
  loopStopKey();

  checkStateTransition();
  forwardLoop();

  updateDisplayLoop();

  // Update Output
  displayManager.loop();
}

void MasterControl::checkStateTransition() {
  switch (uiMode) {
    case UIMode::IDLE:
    case UIMode::LOCOCONTROL:
    case UIMode::LOCOLIST:
      break;
    case UIMode::LOCODOWNLOAD:
      // Switch to control when an engine was downloaded.
      if (locoControl.hasValidEngine()) {
        enterLocoControl();
      }
  }
}

void MasterControl::forwardLoop() {
  model::InputState& inputState = input.getInputState();

  // Depending on mode, loop the associated control
  switch (uiMode) {
    case UIMode::IDLE:
      idleControl.loop(inputState, *this);
      break;
    case UIMode::LOCOCONTROL:
    case UIMode::LOCODOWNLOAD:
      locoControl.loop(inputState, *this);
      break;
    case UIMode::LOCOLIST:
      locoList.loop(inputState, *this);
      break;
  }
}

void MasterControl::enterIdle() {
  printf("UIControl::enterIdle\n");
  uiMode = UIMode::IDLE;
  idleControl.updateDisplayOnce(displayManager);
}

void MasterControl::enterLocoControl() {
  printf("UIControl::enterLocoControl\n");
  if (!locoControl.hasValidEngine()) {
    enterIdle();
  } else {
    displayManager.disableCursor();
    uiMode = UIMode::LOCOCONTROL;
    locoControl.enterLocoControl(input.getInputState());
    locoControl.requestLocoData();
    locoControl.updateDisplayOnce(displayManager);
  }
}

void MasterControl::enterLocoList() {
  printf("UIControl::enterLocoList\n");
  uiMode = UIMode::LOCOLIST;
  input.getInputState().loadEncoderPosition(locoList.getCursorPosition());
  locoList.RequestDownloadAtCursor();
  displayManager.disableCursor();
}

void MasterControl::enterLocoDownload() {
  printf("UIControl::enterLocoDownload\n");
  uiMode = UIMode::LOCODOWNLOAD;

  // Copy selected engine from locoList to locoControl and start the download of
  // data
  const RR32Can::LocomotiveShortInfo* locoInfo = locoList.getSelectedEngine();
  if (locoInfo == nullptr) {
    // No engine was selected, go to IDLE
    enterIdle();
  } else {
    if (!locoControl.setLocoInfo(*locoInfo)) {
      // Previous Engine was reselected, go to another mode
      if (locoControl.hasValidEngine()) {
        enterLocoControl();
      } else {
        enterIdle();
      }
    } else {
      // An actual change was made. Update the UI.
      locoControl.updateDisplayOnce(displayManager);

      // Request the "lokomotive" data.
      locoControl.requestLocoFile();
    }
  }
}

void MasterControl::updateDisplayLoop() {
  switch (uiMode) {
    case UIMode::IDLE:
      // Was set during entering, nothing to do.
      break;
    case UIMode::LOCOLIST:
      // Handled by LOCOLIST itself
      locoList.updateDisplay(displayManager);
      break;
    case UIMode::LOCODOWNLOAD:
    case UIMode::LOCOCONTROL:
      break;
  }

  // Always update the non-text loco artifacts
  locoControl.updateDisplayLoop(displayManager);
}

void MasterControl::loopStopKey() {
  // Read the STOP button
  model::InputState& inputState = input.getInputState();

  if (inputState.isStopRisingEdge()) {
    if (displayManager.getSystemOn()) {
      RR32Can::RR32Can.SendSystemStop();
    } else {
      RR32Can::RR32Can.SendSystemGo();
    }
  }
}

RR32Can::Locomotive* MasterControl::getLoco(
    RR32Can::Locomotive::Uid_t engineUid) {
  RR32Can::Locomotive& loco = locoControl.getLoco();
  if (!loco.isFullDetailsKnown()) {
    return nullptr;
  }
  if (loco.getUidMasked() != engineUid) {
    return nullptr;
  }

  return &loco;
}

void MasterControl::setLocoVelocity(RR32Can::Locomotive::Uid_t uid,
                                    RR32Can::Velocity_t velocity) {
  if (getLoco(uid) != 0) {
    locoControl.setReceivedVelocity(velocity, *this);
  }
}

void MasterControl::setLocoVelocity(RR32Can::Velocity_t velocity) {
  locoControl.setReceivedVelocity(velocity, *this);
}

}  // namespace controller

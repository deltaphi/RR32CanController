#include "application/controller/Idle.h"
#include <application/controller/MasterControl.h>

#include "application/view/UIAssets.h"

namespace application {
namespace controller {

void Idle::loop(application::model::InputState& inputState, MasterControl& masterControl) {
  if (inputState.isEncoderRisingEdge() && inputState.isShiftPressed()) {
    // switch to locolist on shift+encoder
    masterControl.enterLocoList();
  }
}

void Idle::updateDisplayOnce(application::model::DisplayModel& displayManager) {
  strncpy(displayManager.getWritableBuffer(0), application::view::kNoEngine,
          STRING_CHAR_LENGTH);
  strncpy(displayManager.getWritableBuffer(1), application::view::kRR32Can,
          STRING_CHAR_LENGTH);
  displayManager.disableCursor();
}

}  // namespace controller
}  // namespace application

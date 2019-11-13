#include "controller/Idle.h"

#include "controller/UIControl.h"
#include "view/UIAssets.h"

namespace controller {

void Idle::loop(model::InputState& inputState, UIControl& uiControl) {
  if (inputState.isEncoderRisingEdge() && inputState.isShiftPressed()) {
    // switch to locolist on shift+encoder
    uiControl.enterLocoList();
  }
}

void Idle::updateDisplayOnce(view::DisplayManager& displayManager) {
  strncpy(displayManager.getWritableBuffer(0), view::kNoEngine,
          STRING_CHAR_LENGTH);
  strncpy(displayManager.getWritableBuffer(1), view::kRR32Can,
          STRING_CHAR_LENGTH);
  displayManager.disableCursor();
}

}  // namespace controller

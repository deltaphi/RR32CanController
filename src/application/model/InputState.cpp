#include "application/model/InputState.h"

namespace application {
namespace model {

void InputState::reset() {
  for (uint8_t i = TURNOUT_BUTTONS_OFFSET; i < TURNOUT_BUTTONS_COUNT + TURNOUT_BUTTONS_OFFSET;
       ++i) {
    keys[i].forceDebounce(LOW);
    keys[i].getAndResetEdgeFlag();
  }

  for (uint8_t i = 0; i < TURNOUT_BUTTONS_OFFSET; ++i) {
    keys[i].forceDebounce(HIGH);
    keys[i].getAndResetEdgeFlag();
  }

  encoder.setPosition(0);
}

void InputState::resetAllEdges() {
  for (uint8_t i = 0; i < MODEL_INPUTSTATE_KEYARRAY_LENGTH; ++i) {
    keys[i].getAndResetEdgeFlag();
  }
}

}  // namespace model
}  // namespace application

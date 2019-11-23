#include "model/InputState.h"

namespace model {

void InputState::reset() {
  for (uint8_t i = 0; i < MODEL_INPUTSTATE_KEYARRAY_LENGTH; ++i) {
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

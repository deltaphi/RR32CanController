#include "model/InputState.h"

namespace model {

const uint8_t InputState::kKeyGpio[MODEL_INPUTSTATE_KEYARRAY_LENGTH] = {
    F0_BUTTON_PIN,      F1_BUTTON_PIN,   F2_BUTTON_PIN,   F3_BUTTON_PIN,
    ENCODER_BUTTON_PIN, STOP_BUTTON_PIN, SHIFT_BUTTON_PIN};

void InputState::reset() {
  for (uint8_t i = 0; i < MODEL_INPUTSTATE_KEYARRAY_LENGTH; ++i) {
    keys[i].forceDebounce(HIGH);
    keys[i].getAndResetEdgeFlag();
  }
  encoder.setPosition(0);
}

}  // namespace model

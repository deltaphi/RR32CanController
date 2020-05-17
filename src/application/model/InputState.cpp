#include "application/model/InputState.h"

namespace application {
namespace model {

void initKey(InputState::Key_t &key, uint8_t initialDebounce) {
  key.forceDebounce(initialDebounce);
  key.getAndResetEdgeFlag();
}

void InputState::reset() {
  initKey(encoderKey, LOW);
  initKey(shift, LOW);
  initKey(stop, LOW);

  for (uint8_t i = 0; i < NUM_FBUTTONS; ++i) {
    initKey(functionKeys[i], LOW);
  }

  for (uint8_t i = 0; i < TURNOUT_BUTTONS_OFFSET; ++i) {
    initKey(turnoutKeys[i], HIGH);
  }

  encoder.setPosition(0);
}

void InputState::resetAllEdges() {
  encoderKey.getAndResetEdgeFlag();
  shift.getAndResetEdgeFlag();
  stop.getAndResetEdgeFlag();

  for (uint8_t i = 0; i < NUM_FBUTTONS; ++i) {
    functionKeys[i].getAndResetEdgeFlag();
  }

  for (uint8_t i = 0; i < TURNOUT_BUTTONS_OFFSET; ++i) {
    turnoutKeys[i].getAndResetEdgeFlag();
  }
}

}  // namespace model
}  // namespace application

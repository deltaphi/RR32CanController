#include "controller/Input.h"

namespace controller {

void Input::begin() {
  // Initialize data structure
  inputState.reset();

  // Initialize direct GPIO pins
  pinMode(ENCODER_BUTTON_PIN, INPUT);
  pinMode(STOP_BUTTON_PIN, INPUT);
  pinMode(SHIFT_BUTTON_PIN, INPUT);

  pinMode(F0_BUTTON_PIN, INPUT);
  pinMode(F1_BUTTON_PIN, INPUT);
  pinMode(F2_BUTTON_PIN, INPUT);
  pinMode(F3_BUTTON_PIN, INPUT);
  // pinMode(F4_BUTTON_PIN, INPUT);

  // TODO: intialize shift register
}

void Input::loopEncoder() { inputState.encoder.tick(); }

void Input::loopButtons() {
  for (uint8_t i = 0; i < MODEL_INPUTSTATE_KEYARRAY_LENGTH; ++i) {
    uint8_t readValue = digitalRead(inputState.kKeyGpio[i]);
    inputState.keys[i].cycle(readValue);
  }
}

void Input::loopShiftRegister() {}

}  // namespace controller

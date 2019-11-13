#ifndef __CONTROLLER__INPUT_H__
#define __CONTROLLER__INPUT_H__

#include "model/InputState.h"

namespace controller {

/*
 * \brief Class Input.
 *
 * Reads the encoder and all buttons (shift register or otherwise)
 */
class Input {
 public:
  void begin();

  void loop() {
    loopEncoder();
    loopButtons();
    loopShiftRegister();
  }

  model::InputState& getInputState() { return inputState; }

 private:
  void loopEncoder();
  void loopButtons();
  void loopShiftRegister();

  model::InputState inputState;
};

}  // namespace controller

#endif  // __CONTROLLER__INPUT_H__

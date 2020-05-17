#ifndef __HAL__INPUT_H__
#define __HAL__INPUT_H__

#include "AsyncShiftIn.h"

#include "application/model/InputState.h"

namespace hal {

/*
 * \brief Class Input.
 *
 * Reads the encoder and all buttons (shift register or otherwise)
 */
class Input {
 public:
  void begin(application::model::InputState& inputState);

  void loop() {
    loopEncoder();
    loopShiftRegister();
    loopGpio();
  }

#if (SHIFT_REGISTER_LENGTH > 0)
  void shiftIn_reset(const AsyncShiftIn* shiftIn);
  void shiftIn_shift(const AsyncShiftIn* asyncShiftIn, unsigned int bitNumber, uint8_t state);
#endif

 private:
  void loopEncoder();
  void loopShiftRegister();
  void loopGpio();

#if (SHIFT_REGISTER_LENGTH > 0)
  /**
   * \brief Variable stating whether the shift register has been initialized.
   */
  boolean initialized;

  /**
   * \brief The shift register.
   */
  AsyncShiftIn shiftRegister0;
#endif

  application::model::InputState* inputState;
};

}  // namespace hal

#endif  // __HAL__INPUT_H__

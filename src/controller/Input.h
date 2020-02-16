#ifndef __CONTROLLER__INPUT_H__
#define __CONTROLLER__INPUT_H__

#include "config.h"

#if (SHIFT_REGISTER_LENGTH > 0)
#include "AsyncShiftIn.h"
#endif

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
#if (SHIFT_REGISTER_LENGTH > 0)
    loopShiftRegister();
#endif
#if (FBUTTONS_ON_SHIFTREGISTER == STD_ON)
    loopButtons();
#endif
  }

  model::InputState& getInputState() { return inputState; }

#if (SHIFT_REGISTER_LENGTH > 0)
  void shiftIn_reset(const AsyncShiftIn* shiftIn);
  void shiftIn_shift(const AsyncShiftIn* asyncShiftIn, unsigned int bitNumber,
                     uint8_t state);
#endif

 private:
  void loopEncoder();

#if (SHIFT_REGISTER_LENGTH > 0)
  void loopShiftRegister();
#endif

#if (FBUTTONS_ON_SHIFTREGISTER == STD_ON)
  void loopButtons();
#endif

  /**
   * \brief Variable stating whether the shift register has been initialized.
   */
  boolean initialized;

#if (SHIFT_REGISTER_LENGTH > 0)
  /**
   * \brief The shift register.
   */
  AsyncShiftIn shiftRegister0;
#endif

  model::InputState inputState;
};

}  // namespace controller

#endif  // __CONTROLLER__INPUT_H__

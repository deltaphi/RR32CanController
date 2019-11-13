#ifndef __MODEL__INPUTSTATE_H__
#define __MODEL__INPUTSTATE_H__

#include <RotaryEncoder.h>
#include "DebouncedDualKey.h"

#include "config.h"

#define MODEL_INPUTSTATE_KEYARRAY_LENGTH (NUM_FBUTTONS + 3)

namespace model {

/*
 * \brief Class InputState
 */
class InputState {
 public:
  using EncoderPosition_t = long;

  struct EncoderInfo_t {
    EncoderPosition_t position;
    RotaryEncoder::Direction direction;
  };

  using Key_t = DebouncedKey<1, 1>;

  /**
   * \brief Contains all non-turnout button states.
   *
   * 0..n: Function keys
   * n+1: encoder button
   * n+2: stop button
   * n+3: shift button
   */
  using KeyFullArray_t = Key_t[MODEL_INPUTSTATE_KEYARRAY_LENGTH];

  constexpr static const uint8_t kShiftKeyIndex = NUM_FBUTTONS + 2;
  constexpr static const uint8_t kStopKeyIndex = NUM_FBUTTONS + 1;
  constexpr static const uint8_t kEncoderKeyIndex = NUM_FBUTTONS;

  void reset();

  /// Returns whether shift is currently held
  bool isShiftPressed() const {
    return keys[kShiftKeyIndex].getDebouncedValue() == LOW;
  }

  bool isEncoderRisingEdge() {
    return keys[kEncoderKeyIndex].getAndResetEdgeFlag() &&
           keys[kEncoderKeyIndex].getDebouncedValue() == LOW;
  }

  /// Returns whether there was a rising edge on the STOP button since the last
  /// check.
  bool isStopRisingEdge() {
    return keys[kStopKeyIndex].getAndResetEdgeFlag() &&
           keys[kStopKeyIndex].getDebouncedValue() == LOW;
  }

  Key_t* getControlKeys() {
#if (NUM_FBUTTONS == 0)
    return nullptr;
#else
    return &keys[NUM_FBUTTONS];
#endif
  }

  Key_t* getFunctionKeys() { return keys; }

  Key_t* getTurnoutKeys() { return nullptr; };

  void loadEncoderPosition(EncoderPosition_t position) {
    lastEncoderState.position = position;
    lastEncoderState.direction = RotaryEncoder::Direction::NOROTATION;
    encoder.setPosition(position);
  }

  KeyFullArray_t keys;

  /// Contains the last position read from the encoder.
  EncoderInfo_t lastEncoderState;
  RotaryEncoder encoder{ENCODER_A_PIN, ENCODER_B_PIN};

  static const uint8_t kKeyGpio[MODEL_INPUTSTATE_KEYARRAY_LENGTH];
};

}  // namespace model

#endif  // __MODEL__INPUTSTATE_H__
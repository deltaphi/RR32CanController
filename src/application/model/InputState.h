#ifndef __APPLICATION__MODEL__INPUTSTATE_H__
#define __APPLICATION__MODEL__INPUTSTATE_H__

#include <RotaryEncoder.h>

#include <Arduino.h>
#include <array>

#include "DebouncedDualKey.h"

#include "config.h"

namespace application {
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

  using FunctionKeyArray_t = std::array<Key_t, NUM_FBUTTONS>;
  using TurnoutKeyArray_t = std::array<Key_t, TURNOUT_BUTTONS_COUNT>;

  void reset();

  void resetAllEdges();

  /// Returns whether shift is currently held
  bool isShiftPressed() const {
    return shift.getDebouncedValue() == HIGH;  // For some reason, SHIFT has to be inverted.
  }

  bool isEncoderRisingEdge() {
    return encoderKey.getAndResetEdgeFlag() && encoderKey.getDebouncedValue() == LOW;
  }

  /// Returns whether there was a rising edge on the STOP button since the last
  /// check.
  bool isStopRisingEdge() { return stop.getAndResetEdgeFlag() && stop.getDebouncedValue() == LOW; }

  FunctionKeyArray_t& getFunctionKeys() { return functionKeys; }

  TurnoutKeyArray_t& getTurnoutKeys() { return turnoutKeys; }

  Key_t& getEncoderKey() { return encoderKey; }
  Key_t& getStopKey() { return stop; }
  Key_t& getShiftKey() { return shift; }

  void loadEncoderPosition(EncoderPosition_t position) {
    lastEncoderState.position = position;
    lastEncoderState.direction = RotaryEncoder::Direction::NOROTATION;
    encoder.setPosition(position);
  }

  bool isEncoderMoved() { return lastEncoderState.position != encoder.getPosition(); }

  // Sets the lastEncoderState to the current Encoder values.
  void consumeEncoderPosition() {
    lastEncoderState.position = encoder.getPosition();
    lastEncoderState.direction = encoder.getDirection();
  }

  /// Contains the last position read from the encoder.
  EncoderInfo_t lastEncoderState;
  RotaryEncoder encoder{ENCODER_A_PIN, ENCODER_B_PIN};

 private:
  Key_t encoderKey;
  Key_t shift;
  Key_t stop;
  FunctionKeyArray_t functionKeys;
  TurnoutKeyArray_t turnoutKeys;
};

}  // namespace model
}  // namespace application

#endif  // __APPLICATION__MODEL__INPUTSTATE_H__

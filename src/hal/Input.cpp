#include "hal/Input.h"

#include "config.h"

namespace hal {

/**
 * Used to reroute the callbacks from AsyncShiftIn to this class.
 */
static Input* inputPtr = nullptr;

void Input::begin(application::model::InputState& inputState) {
  // Initialize data structure
  this->inputState = &inputState;
  inputPtr = this;

  // intialize shift register
  initialized = false;
  shiftRegister0.init(S88_DATA_IN_PIN, S88_CLOCK_OUT_PIN, S88_PS_OUT_PIN,
                      S88_RESET_OUT_PIN, SHIFT_REGISTER_LENGTH, 90, 50);
  Serial.println(F("Initialized Shift Register ingerface."));

  // Initialize Keys and perform a full sweep of the shift register
  while (!initialized) {
    shiftRegister0.loop();
  }
  // Reset all edges
  inputState.resetAllEdges();
  Serial.println(F("Initial full sweep done."));
}

void Input::loopEncoder() { inputState->encoder.tick(); }

void Input::loopShiftRegister() {
  // Process the shift register
  shiftRegister0.loop();
}

/**
 * \brief Callback called by AsyncShiftIn::loop() once a loop hs been completed.
 */
void Input::shiftIn_reset(const AsyncShiftIn* asyncShiftIn) {
  // Any reset, especially the first one, will cause initialized to be set to
  // TRUE
  initialized = true;

#if (LOG_S88_BITS == STD_ON)
  Serial.println(F(" RESET! "));
#endif

#if (LOG_KEYS_DEBOUNCE == STD_ON)
  // Print the debounced Keys
  for (int i = 0; i < SHIFT_REGISTER_LENGTH; ++i) {
    Serial.print(i);
    Serial.print((TurnoutControl::keyArray[i].getDebouncedValue() == HIGH
                      ? ":HIGH/CLOSE "
                      : ":LOW/PRESS "));
  }
  Serial.println();
#endif
}

/**
 * \brief Callback called by AsyncShiftIn::loop() every time a bit has been
 * read.
 */
void Input::shiftIn_shift(const AsyncShiftIn* asyncShiftIn,
                          unsigned int bitNumber, uint8_t state) {
#if (LOG_S88_BITS == STD_ON)
  // Serial.print(bitNumber);
  // Serial.print((state == HIGH ? ":HIGH " : ":LOW  "));
  Serial.print((state == HIGH ? "1" : "0"));
#endif

  // We read an inverted input - invert state
  state = (state == HIGH ? LOW : HIGH);

  // set the key debounce algorithm.
#if (KEYS_DEBOUNCE == STD_ON)
  inputState.keys[bitNumber].cycle(state);
#else
  inputState->keys[bitNumber].forceDebounce(state);
#endif
}

}  // namespace hal

void AsyncShiftIn_reset(const AsyncShiftIn* asyncShiftIn) {
  if (hal::inputPtr != nullptr) {
    hal::inputPtr->shiftIn_reset(asyncShiftIn);
  }
}

void AsyncShiftIn_shift(const AsyncShiftIn* asyncShiftIn,
                        unsigned int bitNumber, uint8_t state) {
  if (hal::inputPtr != nullptr) {
    hal::inputPtr->shiftIn_shift(asyncShiftIn, bitNumber, state);
  }
}

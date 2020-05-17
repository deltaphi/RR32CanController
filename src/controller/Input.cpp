#include "controller/Input.h"

#include "config.h"

namespace controller {

/**
 * Used to reroute the callbacks from AsyncShiftIn to this class.
 */
static Input* inputPtr = nullptr;

void Input::begin() {
  inputPtr = this;
  // Initialize data structure
  inputState.reset();

  // intialize shift register
  initialized = false;
#if (SHIFT_REGISTER_LENGTH > 0)
  shiftRegister0.init(S88_DATA_IN_PIN, S88_CLOCK_OUT_PIN, S88_PS_OUT_PIN,
                      S88_RESET_OUT_PIN, SHIFT_REGISTER_LENGTH, 90, 50);
  Serial.println(F("Initialized Shift Register ingerface."));

  // Initialize Keys and perform a full sweep of the shift register
  while (!initialized) {
    shiftRegister0.loop();
  }
#endif

#if (FBUTTONS_ON_SHIFTREGISTER == STD_ON)
  for (uint8_t i = 0; i < NUM_FBUTTONS; ++i) {
    pinMode(kPinsFunction[i], INPUT);
  }

  pinMode(PIN_SHIFT, INPUT);
  pinMode(PIN_STOP, INPUT);
  pinMode(PIN_ENCODER, INPUT);
#endif

  // Reset all edges
  inputState.resetAllEdges();
  Serial.println(F("Initial full sweep done."));
}

void Input::loopEncoder() { inputState.encoder.tick(); }

#if (SHIFT_REGISTER_LENGTH > 0)
void Input::loopShiftRegister() {
  // Process the shift register
  shiftRegister0.loop();
}
#endif

#if (FBUTTONS_ON_SHIFTREGISTER == STD_ON)
void Input::loopButtons() {
  model::InputState::Key_t* keys = inputState.getFunctionKeys();

  for (uint8_t i = 0; i < NUM_FBUTTONS; ++i) {
    int value = digitalRead(kPinsFunction[i]);
    keys[i].forceDebounce(value);
#if (LOG_BUTTON_PRESS == STD_ON)
    if (keys[i].hasEdge()) {
      printf("Button F%i is %s\n", i,
             (keys[i].getDebouncedValue() == HIGH ? "HIGH" : "LOW"));
    }
#endif
  }

  int shiftOld = keys[model::InputState::kShiftKeyIndex].getDebouncedValue();
  keys[model::InputState::kShiftKeyIndex].forceDebounce(digitalRead(PIN_SHIFT));
#if (LOG_BUTTON_PRESS == STD_ON)
  if (keys[model::InputState::kShiftKeyIndex].getAndResetEdgeFlag()) {
    printf("Button SHIFT is %s (was %s)\n",
           (keys[model::InputState::kShiftKeyIndex].getDebouncedValue() == HIGH
                ? "HIGH"
                : "LOW"),
           (shiftOld == HIGH ? "HIGH" : "LOW"));
  }
#endif

  keys[model::InputState::kStopKeyIndex].forceDebounce(digitalRead(PIN_STOP));
#if (LOG_BUTTON_PRESS == STD_ON)
  if (keys[model::InputState::kStopKeyIndex].hasEdge()) {
    printf("Button STOP is %s\n",
           (keys[model::InputState::kStopKeyIndex].getDebouncedValue() == HIGH
                ? "HIGH"
                : "LOW"));
  }
#endif

  keys[model::InputState::kEncoderKeyIndex].forceDebounce(
      digitalRead(PIN_ENCODER));
#if (LOG_BUTTON_PRESS == STD_ON)
  if (keys[model::InputState::kEncoderKeyIndex].hasEdge()) {
    printf(
        "Button ENCODER is %s\n",
        (keys[model::InputState::kEncoderKeyIndex].getDebouncedValue() == HIGH
             ? "HIGH"
             : "LOW"));
  }
#endif
}

#endif

#if (SHIFT_REGISTER_LENGTH > 0)
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
  inputState.keys[bitNumber].forceDebounce(state);
#endif
}
#endif

}  // namespace controller

#if (SHIFT_REGISTER_LENGTH > 0)
void AsyncShiftIn_reset(const AsyncShiftIn* asyncShiftIn) {
  if (controller::inputPtr != nullptr) {
    controller::inputPtr->shiftIn_reset(asyncShiftIn);
  }
}

void AsyncShiftIn_shift(const AsyncShiftIn* asyncShiftIn,
                        unsigned int bitNumber, uint8_t state) {
  if (controller::inputPtr != nullptr) {
    controller::inputPtr->shiftIn_shift(asyncShiftIn, bitNumber, state);
  }
}
#endif
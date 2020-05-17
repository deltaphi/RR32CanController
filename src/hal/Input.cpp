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

#if (SHIFT_REGISTER_LENGTH > 0)
  // intialize shift register
  initialized = false;
  shiftRegister0.init(S88_DATA_IN_PIN, S88_CLOCK_OUT_PIN, S88_PS_OUT_PIN, S88_RESET_OUT_PIN,
                      SHIFT_REGISTER_LENGTH, 90, 50);
  printf("Initialized Shift Register ingerface.\n");

  // Initialize Keys and perform a full sweep of the shift register
  while (!initialized) {
    shiftRegister0.loop();
  }
#else
  printf("No Shift Register.\n");
#endif

#if (FUNCTION_KEY_MODE == KEY_MODE_GPIO)
  pinMode(KEY_GPIO_ENCODER, INPUT);
  pinMode(KEY_GPIO_SHIFT, INPUT);
  pinMode(KEY_GPIO_STOP, INPUT);
  pinMode(KEY_GPIO_F0, INPUT);
  pinMode(KEY_GPIO_F1, INPUT);
  pinMode(KEY_GPIO_F2, INPUT);
  pinMode(KEY_GPIO_F3, INPUT);
  pinMode(KEY_GPIO_F4, INPUT);
#endif

  // Reset all edges
  inputState.resetAllEdges();
  printf("Initial full sweep done.\n");
}

void Input::loopEncoder() { inputState->encoder.tick(); }

void Input::loopGpio() {
#if (FUNCTION_KEY_MODE == KEY_MODE_GPIO)
  uint8_t value;

  value = digialRead(KEY_GPIO_ENCODER);
  setKey(inputState->getEncoderKey(), value);

  value = digialRead(KEY_GPIO_SHIFT);
  setKey(inputState->getShiftKey(), value);

  value = digialRead(KEY_GPIO_STOP);
  setKey(inputState->getStopKey(), value);

  value = digialRead(KEY_GPIO_F0);
  setKey(inputState->getFunctionKeys()[0], value);

  value = digialRead(KEY_GPIO_F1);
  setKey(inputState->getFunctionKeys()[1], value);

  value = digialRead(KEY_GPIO_F2);
  setKey(inputState->getFunctionKeys()[2], value);

  value = digialRead(KEY_GPIO_F3);
  setKey(inputState->getFunctionKeys()[3], value);

  value = digialRead(KEY_GPIO_F4);
  setKey(inputState->getFunctionKeys()[4], value);
#endif
}

void Input::loopShiftRegister() {
  // Process the shift register
#if (SHIFT_REGISTER_LENGTH > 0)
  shiftRegister0.loop();
#endif
}

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
    Serial.print(
        (TurnoutControl::keyArray[i].getDebouncedValue() == HIGH ? ":HIGH/CLOSE " : ":LOW/PRESS "));
  }
  Serial.println();
#endif
}

/**
 * Helper Function to set key values with the proper debouncing algorithm
 */
void setKey(application::model::InputState::Key_t& key, uint8_t state) {
#if (KEYS_DEBOUNCE == STD_ON)
  key.cycle(state);
#else
  key.forceDebounce(state);
#endif
}

/**
 * \brief Callback called by AsyncShiftIn::loop() every time a bit has been
 * read.
 */
void Input::shiftIn_shift(const AsyncShiftIn* asyncShiftIn, unsigned int bitNumber, uint8_t state) {
#if (LOG_S88_BITS == STD_ON)
  // Serial.print(bitNumber);
  // Serial.print((state == HIGH ? ":HIGH " : ":LOW  "));
  Serial.print((state == HIGH ? "1" : "0"));
#endif

  // We read an inverted input - invert state
  state = (state == HIGH ? LOW : HIGH);

  // Apply Key Mapping: Map Bit number to inputState key.
  // Is this in the range for turnouts?
  if (bitNumber >= TURNOUT_BUTTONS_OFFSET &&
      bitNumber < TURNOUT_BUTTONS_OFFSET + TURNOUT_BUTTONS_COUNT) {
    bitNumber -= TURNOUT_BUTTONS_OFFSET;
    application::model::InputState::TurnoutKeyArray_t& turnoutKeys = inputState->getTurnoutKeys();
    setKey(turnoutKeys[bitNumber], state);
  } else {
    // Should be a function key
#if (FUNCTION_KEY_MODE == KEY_MODE_SHIFTREGISTER)
    if (bitNumber >= KEY_BIT_F_OFFSET && bitNumber < KEY_BIT_F_OFFSET + KEY_BIT_F_COUNT) {
      bitNumber += KEY_BIT_F_OFFSET;
      application::model::InputState::FunctionKeyArray_t& functionKeys =
          inputState->getFunctionKeys();
      setKey(functionKeys[bitNumber], state);
    } else {
      switch (bitNumber) {
        case KEY_BIT_ENCODER:
          setKey(inputState->getEncoderKey(), state);
          break;
        case KEY_BIT_STOP:
          setKey(inputState->getStopKey(), state);
          break;
        case KEY_BIT_SHIFT:
          setKey(inputState->getShiftKey(), state);
          break;
      }
    }
#endif
  }
}

#endif  // SHIFT_REGISTER_LENGTH

}  // namespace hal

#if (SHIFT_REGISTER_LENGTH > 0)
void AsyncShiftIn_reset(const AsyncShiftIn* asyncShiftIn) {
  if (hal::inputPtr != nullptr) {
    hal::inputPtr->shiftIn_reset(asyncShiftIn);
  }
}

void AsyncShiftIn_shift(const AsyncShiftIn* asyncShiftIn, unsigned int bitNumber, uint8_t state) {
  if (hal::inputPtr != nullptr) {
    hal::inputPtr->shiftIn_shift(asyncShiftIn, bitNumber, state);
  }
}
#endif  // SHIFT_REGISTER_LENGTH

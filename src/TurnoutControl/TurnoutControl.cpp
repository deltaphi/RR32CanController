#include <CAN.h>

#include "TurnoutControl/TurnoutControl.h"

#include "RR32Can/Constants.h"
#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"
#include "RR32Can/messages/TurnoutPacket.h"
#include "RR32Can/handler.h"

namespace TurnoutControl {

boolean initialized = false;
AsyncShiftIn shiftRegister0;
DebouncedKeyArray keyArray;
ActionListProcessor actionListProcessor;

void begin() {
  // Initializing the shift register
  initialized = false;
  shiftRegister0.init(S88_DATA_IN_PIN, S88_CLOCK_OUT_PIN, S88_PS_OUT_PIN,
                      S88_RESET_OUT_PIN, SHIFT_REGISTER_LENGTH, 90, 50);
  Serial.println(F("Initialized Shift Register ingerface."));

  // Initialize Keys and perform a full sweep of the shift register
  for (int i = 0; i < SHIFT_REGISTER_LENGTH; ++i) {
    keyArray[i].forceDebounce(LOW);
  }
  while (!initialized) {
    shiftRegister0.loop();
  }
  Serial.println(F("Initial full sweep done."));
}

void loop() {
  // Process the shift register
  shiftRegister0.loop();

  // Process active action lists
  actionListProcessor.loop();
}

void handleMultiturnout(TurnoutLookupResult result,
                        RR32Can::TurnoutDirection requestedDirection) {
  --result.address;  // Simple mapping to index into actionLists
  constexpr uint8_t actionListEndIndex =
      (NumActionLists /
       ((uint8_t)2U));  // Index one past the end of half the action lists
  if (result.address >= actionListEndIndex) {
    Serial.print("Requested Action List ");
    Serial.print(result.address, DEC);
    Serial.println(" is out of range.");
    return;
  }

  if (requestedDirection == RR32Can::TurnoutDirection::RED) {
    // Add offset into the green lists
    result.address += actionListEndIndex;
  }

  Serial.print("Starting Action List ");
  Serial.println(result.address, DEC);

#if (LOG_ACTIONLIST == STD_ON)
  Serial.print("Requesting action list ");
  Serial.println(result.address, DEC);
#endif
  if (!actionListProcessor.requestActionList(result.address)) {
    Serial.println(
        "An action list is already active. Ignoring request for an additional "
        "one.");
  }
}

/**
 * \brief Called when the debouncing algorithm has determined that a button has
 *        changed position.
 */
void handleButton(uint8_t buttonIndex, uint8_t buttonState) {
  // There was an edge here, send out a message
  TurnoutLookupResult turnoutIndex = lookupTurnout(buttonIndex);

#if (LOG_BUTTON_PRESS == STD_ON)
  Serial.print(F("Button "));
  Serial.print(buttonIndex);
  if (buttonState == HIGH) {
    // Someone pressed this button
    Serial.println(F(" was pressed."));
  } else {
    // Someone released this button
    Serial.println(F(" was released."));
  }
#endif

  switch (turnoutIndex.mode) {
    case TurnoutAddressMode::SingleTurnout: {
      // Single turnout - send out a packet right away.
      RR32Can::TurnoutDirection direction =
          buttonIndex % 2 == 0 ? RR32Can::TurnoutDirection::RED
                               : RR32Can::TurnoutDirection::GREEN;
      direction =
          (direction == RR32Can::TurnoutDirection::RED
               ? RR32Can::TurnoutDirection::GREEN
               : RR32Can::TurnoutDirection::RED);  // invert, as my wires
                                                   // are connected in the
                                                   // opposite order

      RR32Can::SendAccessoryPacket(turnoutIndex.address, direction,
                                   (buttonState == HIGH ? 1 : 0));
      break;
    }
    case TurnoutAddressMode::MultiTurnout: {
      // Only start multi-turnout actions on button press, not release.
      if (buttonState == HIGH) {
        handleMultiturnout(
            turnoutIndex,
            (buttonIndex % 2 == 0 ? RR32Can::TurnoutDirection::RED
                                  : RR32Can::TurnoutDirection::GREEN));
      }
      break;
    }
  }
}

} /* namespace TurnoutControl */

/**
 * \brief Callback called by AsyncShiftIn::loop() once a loop hs been completed.
 */
void AsyncShiftIn_reset(const AsyncShiftIn* asyncShiftIn) {
  // Any reset, especially the first one, will cause initialized to be set to
  // TRUE
  TurnoutControl::initialized = true;

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

  // Now that we have debounced inputs, translate them to turnout requests
  for (int i = 0; i < SHIFT_REGISTER_LENGTH; ++i) {
    if (TurnoutControl::keyArray[i].getAndResetEdgeFlag()) {
      TurnoutControl::handleButton(
          i, TurnoutControl::keyArray[i].getDebouncedValue());
    }
  }
}

/**
 * \brief Callback called by AsyncShiftIn::loop() every time a bit has been
 * read.
 */
void AsyncShiftIn_shift(const AsyncShiftIn* asyncShiftIn,
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
  TurnoutControl::keyArray[bitNumber].cycle(state);
#else
  TurnoutControl::keyArray[bitNumber].forceDebounce(state);
#endif
}

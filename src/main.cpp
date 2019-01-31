#include <Arduino.h>
#include <AsyncShiftIn.h>
#include <CAN.h>

#if (DISPLAY_ATTACHED == STD_ON)
#include <Wire.h>
#include "SSD1306Wire.h"
#endif


#include "ActionListProcessor.h"
#include "DebouncedDualKey.h"
#include "TurnoutTypes.h"
#include "config.h"
#include "maerklin-can.h"

// Whether the shift register has been initialized
boolean initialized;

AsyncShiftIn shiftRegister0;

using DebouncedKeyType = DebouncedKey<10, 0>;

using DebouncedKeyArray = DebouncedKeyType[SHIFT_REGISTER_LENGTH];

DebouncedKeyArray keyArray;

ActionListProcessor actionListProcessor;

#if (DISPLAY_ATTACHED == STD_ON)
SSD1306Wire display(DISPLAY_TWI_ADDRESS, TWI_SDA_PIN, TWI_SCL_PIN);
#endif

void handleMultiturnout(TurnoutLookupResult result,
                        TurnoutDirection requestedDirection) {
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

  if (requestedDirection == TurnoutDirection::RED) {
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
      TurnoutDirection direction = buttonIndex % 2 == 0
                                       ? TurnoutDirection::RED
                                       : TurnoutDirection::GREEN;
      direction =
          (direction == TurnoutDirection::RED
               ? TurnoutDirection::GREEN
               : TurnoutDirection::RED);  // invert, as my wires are connected
                                          // in the opposite order

      sendTurnoutPacket(turnoutIndex.address, direction,
                        (buttonState == HIGH ? 1 : 0));
      break;
    }
    case TurnoutAddressMode::MultiTurnout: {
      // Only start multi-turnout actions on button press, not release.
      if (buttonState == HIGH) {
        handleMultiturnout(turnoutIndex,
                           (buttonIndex % 2 == 0 ? TurnoutDirection::RED
                                                 : TurnoutDirection::GREEN));
      }
      break;
    }
  }
}

void sendTurnoutPacket(uint32_t turnoutAddress, TurnoutDirection direction,
                       uint8_t power) {
  MaerklinCanIdentifier identifier;
  // identifier.prio = 4; // Value is specified but actual implementations don't
  // use it.
  identifier.command = kAccessorySwitch;
  identifier.response = false;
  identifier.computeAndSetHash(maerklinCanUUID);

  MaerklinTurnoutPacket payload;
  payload.locid = turnoutAddress;  // Set the turnout address
  payload.locid |= 0x3000;  // whatever this does. The MS2 does it, though.
  payload.position =
      (direction == TurnoutDirection::RED ? 0
                                          : 1);  // Set the turnout direction
  payload.power = power;

  // Serialize the CAN packet and send it
  MaerklinCanData data;
  payload.serialize(data);

#if (LOG_CAN_OUT_MSG == STD_ON)
  Serial.print("Setting turnout ");
  Serial.print(payload.locid & (~0x3000));
  Serial.print(" to position ");
  Serial.print(payload.position == 0 ? "RED  " : "GREEN ");
  Serial.println(payload.power ? "(ON) " : "(OFF)");
#endif

  // Send packet on CAN
  CAN.beginExtendedPacket(identifier.makeIdentifier());
  for (int i = 0; i < data.dlc; ++i) {
    CAN.write(data.data[i]);
  }
  CAN.endPacket();
}

void setup() {
  // Start serial and wait for its initialization
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("CAN Turnout Control");

  // start the CAN bus at 250 kbps
  if (!CAN.begin(250E3)) {
    Serial.println("Starting CAN failed!");
    // Go into an endless loop if this failed.
    // TODO: Use the on-board LED of the esp32 to signal a failure
    while (1)
      ;
  }

#if (DISPLAY_ATTACHED == STD_ON)
  Serial.println("Starting Display");
  // Initialize the Display
  display.init();
#endif

#if (ENCODER_ENABLED == STD_ON)
  Serial.println("Starting Encoder");
  pinMode(ENCODER_LEFT_PIN, INPUT);
  pinMode(ENCODER_RIGHT_PIN, INPUT);
  pinMode(ENCODER_BUTTON_PIN, INPUT);
#endif

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
#if (DISPLAY_ATTACHED == STD_ON)
  display.clear();

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Hello world");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Hello world");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Hello world");

  display.display();
#endif

  // Process CAN Frames
  int packetSize = CAN.parsePacket();

  if (packetSize > 0) {
    long packetId = CAN.packetId(); /* long is 32 bit */

#if (LOG_CAN_IN_MSG == STD_ON)
    Serial.print(F("Received frame ("));
    if (CAN.packetExtended()) {
      if (CAN.packetRtr()) {
        Serial.print(F("extended, remote)."));
      } else {
        Serial.print(F("extended, data).  "));
      }
    } else {
      if (CAN.packetRtr()) {
        Serial.print(F("regular,  remote). "));
      } else {
        Serial.print(F("regular,  data).   "));
      }
    }
    Serial.print(F(" ID 0x"));
    Serial.print(packetId, HEX);

    Serial.print(" Length: ");
    if (CAN.packetRtr()) {
      Serial.print(CAN.packetDlc());
    } else {
      Serial.print(packetSize);
    }

    Serial.print("Data Bytes (HEX): ");
#endif

    // Read the data bytes into a local buffer.
    // Maerklin does not use remote frames, so we don't care about them.
    MaerklinCanData maerklinData;
    maerklinData.dlc = packetSize;
    for (int i = 0; i < maerklinData.dlc && CAN.available(); ++i) {
      maerklinData.data[i] = (char)CAN.read();
#if (LOG_CAN_IN_MSG == STD_ON)
      Serial.print(" ");
      Serial.print(maerklinData.data[i], HEX);
#endif
    }
#if (LOG_CAN_IN_MSG == STD_ON)
    Serial.println();
#endif

    MaerklinCanIdentifier maerklinIdentifier =
        MaerklinCanIdentifier::GetIdentifier(packetId);
    HandleMaerklinCommand(maerklinIdentifier, maerklinData);
  }

  // Process the shift register
  shiftRegister0.loop();

  // Process active action lists
  actionListProcessor.loop();
}

/**
 * \brief Callback called by AsyncShiftIn::loop() once a loop hs been completed.
 */
void AsyncShiftIn_reset(const AsyncShiftIn* asyncShiftIn) {
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
    Serial.print((keyArray[i].getDebouncedValue() == HIGH ? ":HIGH/CLOSE "
                                                          : ":LOW/PRESS "));
  }
  Serial.println();
#endif

  // Now that we have debounced inputs, translate them to turnout requests
  for (int i = 0; i < SHIFT_REGISTER_LENGTH; ++i) {
    if (keyArray[i].getAndResetEdgeFlag()) {
      handleButton(i, keyArray[i].getDebouncedValue());
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
  keyArray[bitNumber].cycle(state);
#else
  keyArray[bitNumber].forceDebounce(state);
#endif
}

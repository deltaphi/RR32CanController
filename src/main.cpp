#include <Arduino.h>
#include <CAN.h>

#include "EngineControl/EngineControl.h"
#include "TurnoutControl/TurnoutControl.h"

#include "RR32Can/handler.h"

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

  EngineControl::begin();

  TurnoutControl::begin();
}

void CanInputLoop(void);

void loop() {
  EngineControl::loop();

  CanInputLoop();

  TurnoutControl::loop();
}

void CanInputLoop(void) {
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
    RR32Can::Data maerklinData;
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

    RR32Can::Identifier maerklinIdentifier =
        RR32Can::Identifier::GetIdentifier(packetId);
    RR32Can::HandlePacket(maerklinIdentifier, maerklinData);
  }
}

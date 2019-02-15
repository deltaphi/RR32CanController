#include "MaerklinCan/handler.h"

#include <CAN.h>

#include "config.h"

#include "MaerklinCan/TurnoutPacket.h"

namespace MaerklinCan {

void HandlePacket(const MaerklinCan::Identifier & id, const MaerklinCan::Data & data) {
  id.printAll();
  Serial.println();

  Serial.print(F("Command: "));
  switch (id.command) {
    case MaerklinCan::kSystemCommand:
      Serial.print(F("System Command. Subcommand: "));
      switch (data.data[4]) {
        case MaerklinCan::kSubcommandSystemGo:
        Serial.print(F("GO!"));
        //MaerklinSystem.systemOn = true; // TODO: Bring back the System class
        break;
        case MaerklinCan::kSubcommandSystemHalt:
        Serial.print(F("Halt!"));
        break;
        case MaerklinCan::kSubcommandSystemStop:
        Serial.print(F("STOP!"));
        //MaerklinSystem.systemOn = false; // TODO: Bring back the System class
        break;
        case MaerklinCan::kSubcommandSystemIdentifier:
        Serial.print(F("Identifier"));
        break;
        case MaerklinCan::kSubcommandSystemOverload:
        Serial.print(F("OVERLOAD!"));
        break;
        case MaerklinCan::kSubcommandSystemReset:
        Serial.print(F("Reset"));
        //MaerklinSystem.systemOn = false; // TODO: Bring back the System class
        break;
        case MaerklinCan::kSubcommandSystemStatus:
        Serial.print(F("Status"));
        break;
        default:
        Serial.print(F("unknown"));
        break;
      }
      Serial.println();
      break;

    case MaerklinCan::kPing:
      Serial.print(F("Ping. Payload: 0x"));
      data.printAsHex();
      break;

    case MaerklinCan::kAccessorySwitch: 
      Serial.print(F("Accessory Switch. Details: "));
      HandleAccessoryPacket(data);
      break;

    case MaerklinCan::kRequestConfigData:
      Serial.print(F("Request Config Data. Payload: "));
      data.printAsText();
      break;

    case MaerklinCan::kConfigDataStream:
      Serial.print(F("Config Data Stream. Payload: "));
      data.printAsText();
      break;

    default:
      Serial.print(F("unknown or not implemented. Dump: 0x"));
      Serial.print(id.command, HEX);
      data.printAsHex();
      break;
  }
  Serial.println();
}

void HandleAccessoryPacket(const MaerklinCan::Data & data) {
  MaerklinCan::TurnoutPacket turnoutPacket = MaerklinCan::TurnoutPacket::FromCanPacket(data);
  turnoutPacket.printAll();
}


void SendPacket(const MaerklinCan::Identifier& id,
                           const MaerklinCan::Data& data) {
  // Send packet on CAN
  CAN.beginExtendedPacket(id.makeIdentifier());
  for (int i = 0; i < data.dlc; ++i) {
    CAN.write(data.data[i]);
  }
  CAN.endPacket();
}

void SendAccessoryPacket(uint32_t turnoutAddress, TurnoutDirection direction,
                       uint8_t power) {
  MaerklinCan::Identifier identifier;
  // identifier.prio = 4; // Value is specified but actual implementations don't
  // use it.
  identifier.command = MaerklinCan::kAccessorySwitch;
  identifier.response = false;
  identifier.computeAndSetHash(maerklinCanUUID);

  MaerklinCan::TurnoutPacket payload;
  payload.locid = turnoutAddress;  // Set the turnout address
  payload.locid |= 0x3000;  // whatever this does. The MS2 does it, though.
  payload.position =
      (direction == TurnoutDirection::RED ? 0
                                          : 1);  // Set the turnout direction
  payload.power = power;

  // Serialize the CAN packet and send it
  MaerklinCan::Data data;
  payload.serialize(data);
                  
#if (LOG_CAN_OUT_MSG == STD_ON)
  Serial.print("Setting turnout ");
  Serial.print(payload.locid & (~0x3000));
  Serial.print(" to position ");
  Serial.print(payload.position == 0 ? "RED  " : "GREEN ");
  Serial.println(payload.power ? "(ON) " : "(OFF)");
#endif

  SendPacket(identifier, data);

}

void SendRequestConfigDataPacket(const char* data, uint8_t charCount) {
  // Just try to download the first two engines from the MS2
  MaerklinCan::Identifier identifier;
  // identifier.prio = 4; // Value is specified but actual implementations don't
  // use it.
  identifier.command = MaerklinCan::kRequestConfigData;
  identifier.response = false;
  identifier.computeAndSetHash(maerklinCanUUID);

  // Send packet on CAN
  CAN.beginExtendedPacket(identifier.makeIdentifier());
  
  if (charCount > CanDataMaxLength) {
    charCount = CanDataMaxLength;
  }

  for (int i = 0; i < charCount; ++i) {
    CAN.write(data[i]);
  }
  CAN.endPacket();
}

} /* namespace MaerklinCan */

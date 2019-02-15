#include "maerklin-can.h"

#include "MaerklinCan/Data.h"
#include "MaerklinCan/TurnoutPacket.h"

// Global instance of the system
MaerklinSystemClass MaerklinSystem;


void HandleMaerklinCommand(const MaerklinCan::Identifier & id, const MaerklinCan::Data & data) {
  id.printAll();
  Serial.println();

  Serial.print(F("Command: "));
  switch (id.command) {
    case MaerklinCan::kSystemCommand:
      Serial.print(F("System Command. Subcommand: "));
      switch (data.data[4]) {
        case MaerklinCan::kSubcommandSystemGo:
        Serial.print(F("GO!"));
        MaerklinSystem.systemOn = true;
        break;
        case MaerklinCan::kSubcommandSystemHalt:
        Serial.print(F("Halt!"));
        break;
        case MaerklinCan::kSubcommandSystemStop:
        Serial.print(F("STOP!"));
        MaerklinSystem.systemOn = false;
        break;
        case MaerklinCan::kSubcommandSystemIdentifier:
        Serial.print(F("Identifier"));
        break;
        case MaerklinCan::kSubcommandSystemOverload:
        Serial.print(F("OVERLOAD!"));
        break;
        case MaerklinCan::kSubcommandSystemReset:
        Serial.print(F("Reset"));
        MaerklinSystem.systemOn = false;
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
      processAccessory(data);
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


void processAccessory(const MaerklinCan::Data & data) {
  MaerklinCan::TurnoutPacket turnoutPacket = MaerklinCan::TurnoutPacket::FromCanPacket(data);
  turnoutPacket.printAll();
}


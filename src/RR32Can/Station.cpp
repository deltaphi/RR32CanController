#include "RR32Can/Station.h"

#include "RR32Can/EngineBrowser.h"
#include "RR32Can/Handler.h"
#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"
#include "RR32Can/messages/TurnoutPacket.h"
#include "RR32Can/util/utils.h"

#include <Arduino.h>
#include "config.h"

namespace RR32Can {

void Station::begin(uint16_t stationUUID) {
  AbortCurrentConfigRequest();
  senderHash = computeSenderHash(stationUUID);
}

void Station::loop() {}

void Station::FinishCurrentConfigRequest() {
  if (activeConfigDataConsumer != nullptr) {
    activeConfigDataConsumer = nullptr;
  }
  expectedConfigData = ConfigDataStreamType::NONE;
  configDataParser.reset();
}

void Station::AbortCurrentConfigRequest() {
  if (activeConfigDataConsumer != nullptr) {
    activeConfigDataConsumer->setStreamAborted();
  }
  FinishCurrentConfigRequest();
}

void Station::HandleConfigDataStream(const RR32Can::Data& data) {
  if (activeConfigDataConsumer != nullptr) {
#if (LOG_CONFIG_DATA_STREAM_PROCESSING == STD_ON)
    data.printAsHex();
    Serial.print(" '");
    data.printAsText();
    Serial.print("' ");
#endif
    configDataParser.addMessage(data);

  } else {
    // Unknown message or no message expected. Ignore.
    Serial.print("Ignoring ConfigData Stream: ");
    data.printAsText();
  }
}

void Station::RequestEngineList(uint8_t offset) {
  AbortCurrentConfigRequest();

  Identifier id{kRequestConfigData, senderHash};

  /* First packet */
  Data data1;
  data1.dlc = 8;
  strncpy(data1.dataAsString(), EngineBrowser::kFilenameEngineNames,
          Data::kDataBufferLength);

  /* Second packet */
  Data data2;
  data2.dlc = snprintf(data2.dataAsString(), Data::kDataBufferLength, "%d %d",
                       offset, kNumEngineNamesDownload);
  if (data2.dlc <= CanDataMaxLength) {
    expectedConfigData = ConfigDataStreamType::LOKNAMEN;
    activeConfigDataConsumer = &engineBrowser;
    engineBrowser.setStreamOffset(offset);
    configDataParser.startStream(&engineBrowser);
    SendPacket(id, data1);
    SendPacket(id, data2);
  } else {
    // Packet was oversized. Abort the operation.
    return;
  }
}

void Station::RequestEngine(Engine& engine) {
  if (!engine.isNameKnown()) {
#if LOG_CAN_OUT_MSG == STD_ON
    Serial.println("Station::RequestEngine: No Engine Name given, dropping request.");
#endif
    return;  
  }
  
  if (expectedConfigData != ConfigDataStreamType::NONE) {
    /* Given an empty engine slot or a request is already in progress. Abort. */
  #if LOG_CAN_OUT_MSG == STD_ON
    Serial.println("Station::RequestEngine: Request in progress, dropping request.");
  #endif
    return;
  }

  AbortCurrentConfigRequest();

  expectedConfigData = ConfigDataStreamType::LOKINFO;

  Identifier id{kRequestConfigData, senderHash};
  Data data;

  /* First packet */
  data.dlc = 8;
  strncpy(data.dataAsString(), kFilenameEngine, Data::kDataBufferLength);
  SendPacket(id, data);

  /* Second packet */
  strncpy(data.dataAsString(), engine.getName(), CanDataMaxLength);
  data.dlc = strlen(data.dataAsString());  // TODO: Verify of padding with null
                                           // bytes or spaces is required
  SendPacket(id, data);
}

void Station::SendAccessoryPacket(uint32_t turnoutAddress,
                                  TurnoutDirection direction, uint8_t power) {
  RR32Can::Identifier identifier{kAccessorySwitch, this->senderHash};

  RR32Can::TurnoutPacket payload;
  payload.locid = turnoutAddress;  // Set the turnout address
  payload.locid |= 0x3000;  // whatever this does. The MS2 does it, though.
  payload.position =
      (direction == TurnoutDirection::RED ? 0
                                          : 1);  // Set the turnout direction
  payload.power = power;

  // Serialize the CAN packet and send it
  RR32Can::Data data;
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

void Station::HandlePacket(const RR32Can::Identifier& id,
                           const RR32Can::Data& data) {
  id.printAll();
  Serial.println();

  Serial.print(F("Command: "));
  switch (id.command) {
    case RR32Can::kSystemCommand:
      Serial.print(F("System Command. Subcommand: "));
      switch (data.data[4]) {
        case RR32Can::kSubcommandSystemGo:
          Serial.print(F("GO!"));
          // MaerklinSystem.systemOn = true; // TODO: Bring back the System
          // class
          break;
        case RR32Can::kSubcommandSystemHalt:
          Serial.print(F("Halt!"));
          break;
        case RR32Can::kSubcommandSystemStop:
          Serial.print(F("STOP!"));
          // MaerklinSystem.systemOn = false; // TODO: Bring back the System
          // class
          break;
        case RR32Can::kSubcommandSystemIdentifier:
          Serial.print(F("Identifier"));
          break;
        case RR32Can::kSubcommandSystemOverload:
          Serial.print(F("OVERLOAD!"));
          break;
        case RR32Can::kSubcommandSystemReset:
          Serial.print(F("Reset"));
          // MaerklinSystem.systemOn = false; // TODO: Bring back the System
          // class
          break;
        case RR32Can::kSubcommandSystemStatus:
          Serial.print(F("Status"));
          break;
        default:
          Serial.print(F("unknown"));
          break;
      }
      Serial.println();
      break;

    case RR32Can::kPing:
      Serial.print(F("Ping. Payload: 0x"));
      data.printAsHex();
      break;

    case RR32Can::kAccessorySwitch:
      Serial.print(F("Accessory Switch. Details: "));
      this->HandleAccessoryPacket(data);
      break;

    case RR32Can::kRequestConfigData:
      Serial.print(F("Request Config Data. Payload: "));
      data.printAsText();
      break;

    case RR32Can::kConfigDataStream:
      Serial.print(F("Config Data Stream. "));
      this->HandleConfigDataStream(data);
      break;

    default:
      Serial.print(F("unknown or not implemented. Dump: 0x"));
      Serial.print(id.command, HEX);
      data.printAsHex();
      break;
  }
  Serial.println();
}

void Station::HandleAccessoryPacket(const RR32Can::Data& data) {
  RR32Can::TurnoutPacket turnoutPacket =
      RR32Can::TurnoutPacket::FromCanPacket(data);
  turnoutPacket.printAll();
}

}  // namespace RR32Can

#include "RR32Can/Station.h"

#include "RR32Can/Handler.h"
#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"
#include "RR32Can/messages/TurnoutPacket.h"
#include "RR32Can/util/utils.h"

#include <Arduino.h>
#include <RR32Can/LocoConsumer.h>
#include <RR32Can/LocoListConsumer.h>
#include "config.h"

namespace RR32Can {

void Station::begin(uint16_t stationUUID, StationCbk& callback) {
  AbortCurrentConfigRequest();
  this->callback = &callback;
  senderHash = computeSenderHash(stationUUID);
}

void Station::loop() {}

void Station::FinishCurrentConfigRequest() {
  expectedConfigData = ConfigDataStreamType::NONE;
  configDataParser.reset();
}

void Station::AbortCurrentConfigRequest() { FinishCurrentConfigRequest(); }

void Station::HandleConfigDataStream(const RR32Can::Data& data) {
  if (configDataParser.isProcessing()) {
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_ALL)
    data.printAsHex();
    printf(" '");
    data.printAsText();
    printf("' ");
#endif
    configDataParser.addMessage(data);

  } else {
    // Unknown message or no message expected. Ignore.
    Serial.print("Ignoring ConfigData Stream: ");
    data.printAsText();
  }
}

void Station::HandleSystemCommand(const RR32Can::Data& data) {
  Serial.print(F("System Command "));
  if (data.dlc >= 5) {
    Serial.print(" Subcommand: ");
    switch (data.data[4]) {
      case RR32Can::kSubcommandSystemGo:
        Serial.print(F("GO!"));
        callback->setSystemState(true);
        break;
      case RR32Can::kSubcommandSystemHalt: {
        Serial.print(F("Halt!"));
        Locomotive::Uid_t uid = uidFromData(data.data);
        callback->setLocoVelocity(uid, 0);
      } break;
      case RR32Can::kSubcommandSystemStop:
        Serial.print(F("STOP!"));
        callback->setSystemState(false);
        break;
      case kSubcommandLocoEmergencyStop: {
        Locomotive::Uid_t uid = uidFromData(data.data);
        callback->setLocoVelocity(uid, 0);
      } break;
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

  } else {
    // Not a valid command.
    Serial.println();
  }
}

void Station::RequestEngineList(uint8_t offset,
                                RR32Can::LocoListConsumer& configDataConsumer) {
  AbortCurrentConfigRequest();

  Identifier id{kRequestConfigData, senderHash};

  /* First packet */
  Data data1;
  data1.dlc = 8;
  strncpy(data1.dataAsString(), LocoListConsumer::kFilenameEngineNames,
          Data::kDataBufferLength);

  /* Second packet */
  Data data2;
  data2.dlc = snprintf(data2.dataAsString(), Data::kDataBufferLength, "%d %d",
                       offset, kNumEngineNamesDownload);
  if (data2.dlc <= CanDataMaxLength) {
    expectedConfigData = ConfigDataStreamType::LOKNAMEN;
    configDataParser.startStream(&configDataConsumer);
    SendPacket(id, data1);
    SendPacket(id, data2);
  } else {
    // Packet was oversized. Abort the operation.
    return;
  }
}

void Station::RequestEngine(Locomotive& engine,
                            RR32Can::LocoConsumer& configDataConsumer) {
  if (!engine.isNameKnown()) {
#if LOG_CAN_OUT_MSG == STD_ON
    Serial.println(
        "Station::RequestEngine: No Engine Name given, dropping request.");
#endif
    return;
  }

  if (expectedConfigData != ConfigDataStreamType::NONE) {
    /* Given an empty engine slot or a request is already in progress. Abort. */
#if LOG_CAN_OUT_MSG == STD_ON
    Serial.println(
        "Station::RequestEngine: Request in progress, dropping request.");
#endif
    return;
  }

  AbortCurrentConfigRequest();

  expectedConfigData = ConfigDataStreamType::LOKINFO;
  configDataParser.startStream(&configDataConsumer);

  Identifier id{kRequestConfigData, senderHash};
  Data data;

  /* First packet */
  data.dlc = 8;
  strncpy(data.dataAsString(), LocoConsumer::kFilenameEngine,
          Data::kDataBufferLength);
  SendPacket(id, data);

  /* Second packet */
  data.reset();
  data.dlc = 8;
  const char* engineName = engine.getName();
  uint8_t engineNameLength = strlen(engineName);
  strncpy(data.dataAsString(), engineName, CanDataMaxLength);

  SendPacket(id, data);

  /* Third packet */
  data.reset();
  data.dlc = 8;
  if (engineNameLength > CanDataMaxLength) {
    strncpy(data.dataAsString(), engineName + CanDataMaxLength,
            CanDataMaxLength);
  }

  SendPacket(id, data);
}

void Station::uidToData(uint8_t* ptr, Locomotive::Uid_t uid) {
  ptr[0] = static_cast<uint8_t>(uid >> 24);
  ptr[1] = static_cast<uint8_t>(uid >> 16);
  ptr[2] = static_cast<uint8_t>(uid >> 8);
  ptr[3] = static_cast<uint8_t>(uid);
}

void Station::RequestEngineDirection(Locomotive& engine) {
  RR32Can::Identifier identifier{kLocoDirection, this->senderHash};
  RR32Can::Data data;
  data.dlc = 4;
  uidToData(data.data, engine.getUid());

  SendPacket(identifier, data);
}

void Station::SendEngineDirection(Locomotive& engine,
                                  EngineDirection direction) {
  RR32Can::Identifier identifier{kLocoDirection, this->senderHash};
  RR32Can::Data data;
  data.dlc = 5;
  uidToData(data.data, engine.getUid());

  if ((direction == EngineDirection::FORWARD) ||
      (direction == EngineDirection::REVERSE) ||
      (direction == EngineDirection::CHANGE_DIRECTION)) {
    data.data[4] = static_cast<uint8_t>(direction);
    SendPacket(identifier, data);
  }  // else: not implemented.
}

void Station::RequestEngineVelocity(Locomotive& engine) {
  RR32Can::Identifier identifier{kLocoSpeed, this->senderHash};
  RR32Can::Data data;
  data.dlc = 4;
  uidToData(data.data, engine.getUid());

  SendPacket(identifier, data);
}

void Station::SendEngineVelocity(Locomotive& engine,
                                 Locomotive::Velocity_t velocity) {
  RR32Can::Identifier identifier{kLocoSpeed, this->senderHash};
  RR32Can::Data data;
  data.dlc = 6;
  uidToData(data.data, engine.getUid());

  if (velocity > kMaxEngineVelocity) {
    velocity = kMaxEngineVelocity;
  }
  data.data[4] = velocity >> 8;
  data.data[5] = velocity;

  SendPacket(identifier, data);
}

void Station::RequestEngineFunction(Locomotive& engine, uint8_t function) {
  RR32Can::Identifier identifier{kLocoFunction, this->senderHash};
  RR32Can::Data data;
  data.dlc = 5;
  uidToData(data.data, engine.getUid());
  data.data[4] = function;

  SendPacket(identifier, data);
}

void Station::RequestEngineAllFunctions(Locomotive& engine) {
  RR32Can::Identifier identifier{kLocoFunction, this->senderHash};
  RR32Can::Data data;
  data.dlc = 5;
  uidToData(data.data, engine.getUid());
  for (uint8_t i = 0; i < 16; ++i) {
    data.data[4] = i;
    SendPacket(identifier, data);
  }
}

void Station::SendEngineFunction(Locomotive& engine, uint8_t function,
                                 bool value) {
  RR32Can::Identifier identifier{kLocoFunction, this->senderHash};
  RR32Can::Data data;
  data.dlc = 6;
  uidToData(data.data, engine.getUid());
  data.data[4] = function;

  if (value) {
    data.data[5] = 1;
  } else {
    data.data[5] = 0;
  }

  SendPacket(identifier, data);
}

void Station::SendEmergencyStop(Locomotive& engine) {
  RR32Can::Identifier identifier{kSystemCommand, this->senderHash};
  RR32Can::Data data;
  data.dlc = 5;
  uidToData(data.data, engine.getUid());
  data.data[4] = kSubcommandLocoEmergencyStop;
  SendPacket(identifier, data);
}

void Station::SendSystemStop() {
  RR32Can::Identifier identifier{kSystemCommand, this->senderHash};
  RR32Can::Data data;
  data.dlc = 5;
  data.data[4] = kSubcommandSystemStop;
  SendPacket(identifier, data);
}

void Station::SendSystemGo() {
  RR32Can::Identifier identifier{kSystemCommand, this->senderHash};
  RR32Can::Data data;
  data.dlc = 5;
  data.data[4] = kSubcommandSystemGo;
  SendPacket(identifier, data);
}

void Station::SendAccessoryPacket(RR32Can::MachineTurnoutAddress turnoutAddress,
                                  TurnoutDirection direction, uint8_t power) {
  RR32Can::Identifier identifier{kAccessorySwitch, this->senderHash};

  RR32Can::TurnoutPacket payload;
  payload.locid = turnoutAddress.value();  // Set the turnout address
  payload.locid |= 0x3000;  // whatever this does. The MS2 does it, though.
  payload.position = RR32Can::TurnoutDirectionToIntegral<uint8_t>(
      direction);  // Set the turnout direction
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
#if LOG_CAN_RAW_MSG_IN == STD_ON
  id.printAll();
  Serial.println();
#endif

  switch (id.command) {
    case RR32Can::kSystemCommand:
      this->HandleSystemCommand(data);
      break;

    case RR32Can::kPing:
#if (LOG_PING == STD_ON)
      Serial.print(F("Ping. Payload: 0x"));
      data.printAsHex();
      Serial.println();
#endif
      break;

    case RR32Can::kAccessorySwitch:
      Serial.print(F("Accessory Switch. Details: "));
      this->HandleAccessoryPacket(data);
      Serial.println();
      break;

    case RR32Can::kLocoDirection:
      this->HandleLocoDirection(data);
      break;

    case RR32Can::kLocoSpeed:
      this->HandleLocoSpeed(data);
      break;

    case RR32Can::kLocoFunction:
      this->HandleLocoFunction(data);
      break;

    case RR32Can::kRequestConfigData:
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_ALL)
      printf("Request Config Data. Payload: ");
      data.printAsText();
      printf("\n");
#endif
      break;

    case RR32Can::kConfigDataStream:
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_ALL)
      printf("Config Data Stream.\n");
#endif
      this->HandleConfigDataStream(data);
      break;

    default:
      printf("Unknown or not implemented. Dump: 0x%#02x\n", id.command);
      data.printAsHex();
      printf("\n");
      break;
  }
}

void Station::HandleAccessoryPacket(const RR32Can::Data& data) {
  RR32Can::TurnoutPacket turnoutPacket =
      RR32Can::TurnoutPacket::FromCanPacket(data);
  turnoutPacket.printAll();
}

Locomotive::Uid_t Station::uidFromData(const uint8_t* ptr) {
  return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
}

Locomotive* Station::getLocoForData(const RR32Can::Data& data) {
  Locomotive::Uid_t uid = uidFromData(data.data);
  return callback->getLoco(uid);
}

void Station::HandleLocoDirection(const RR32Can::Data& data) {
  if (data.dlc == 5) {
    // response.
    Locomotive* engine = getLocoForData(data);
    if (engine == nullptr) {
      return;
    }

    EngineDirection direction = static_cast<EngineDirection>(data.data[4]);
    if (direction == EngineDirection::FORWARD ||
        direction == EngineDirection::REVERSE) {
      engine->setDirection(direction);
    } else if (direction == EngineDirection::CHANGE_DIRECTION) {
      engine->changeDirection();
    }
  }  // else: requests are ignored.
}

void Station::HandleLocoSpeed(const RR32Can::Data& data) {
  if (data.dlc == 6) {
    Locomotive::Uid_t uid = uidFromData(data.data);
    Locomotive::Velocity_t velocity = (data.data[4] << 8) | data.data[5];
    callback->setLocoVelocity(uid, velocity);
  }
}

void Station::HandleLocoFunction(const RR32Can::Data& data) {
  if (data.dlc == 6) {
    Locomotive* engine = getLocoForData(data);
    if (engine == nullptr) {
      return;
    }
    engine->setFunction(data.data[4], data.data[5]);
  }
}

}  // namespace RR32Can

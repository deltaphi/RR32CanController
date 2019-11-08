#include "RR32Can/Station.h"

#include "RR32Can/EngineBrowser.h"
#include "RR32Can/EngineControl.h"
#include "RR32Can/Handler.h"
#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"
#include "RR32Can/messages/TurnoutPacket.h"
#include "RR32Can/util/utils.h"

#include "EngineControl/EngineControl.h"

#include <Arduino.h>
#include "config.h"

namespace RR32Can {

void Station::begin(uint16_t stationUUID) {
  AbortCurrentConfigRequest();
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

void Station::HandleSystemCommand(const RR32Can::Data& data) {
  Serial.print(F("System Command "));
  if (data.dlc >= 5) {
    Serial.print(" Subcommand: ");
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
      case kSubcommandLocoEmergencyStop: {
        Engine* engine = getLocoForData(data);
        if (engine != nullptr) {
          engine->setVelocity(0);
        }
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
  configDataParser.startStream(&engineControl);

  Identifier id{kRequestConfigData, senderHash};
  Data data;

  /* First packet */
  data.dlc = 8;
  strncpy(data.dataAsString(), EngineControl::kFilenameEngine,
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

void uidToData(uint8_t* ptr, Engine::Uid_t uid) {
  ptr[0] = static_cast<uint8_t>(uid >> 24);
  ptr[1] = static_cast<uint8_t>(uid >> 16);
  ptr[2] = static_cast<uint8_t>(uid >> 8);
  ptr[3] = static_cast<uint8_t>(uid);
}

void Station::RequestEngineDirection(Engine& engine) {
  RR32Can::Identifier identifier{kLocoDirection, this->senderHash};
  RR32Can::Data data;
  data.dlc = 4;
  uidToData(data.data, engine.getUid());

  SendPacket(identifier, data);
}

void Station::SendEngineDirection(Engine& engine, EngineDirection direction) {
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

void Station::RequestEngineVelocity(Engine& engine) {
  RR32Can::Identifier identifier{kLocoSpeed, this->senderHash};
  RR32Can::Data data;
  data.dlc = 4;
  uidToData(data.data, engine.getUid());

  SendPacket(identifier, data);
}

void Station::SendEngineVelocity(Engine& engine, Engine::Velocity_t velocity) {
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

void Station::SendEmergencyStop(Engine& engine) {
  RR32Can::Identifier identifier{kSystemCommand, this->senderHash};
  RR32Can::Data data;
  data.dlc = 5;
  uidToData(data.data, engine.getUid());
  data.data[4] = kSubcommandLocoEmergencyStop;
  SendPacket(identifier, data);
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

    case RR32Can::kRequestConfigData:
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_ALL)
      Serial.print(F("Request Config Data. Payload: "));
      data.printAsText();
      Serial.println();
#endif
      break;

    case RR32Can::kConfigDataStream:
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_ALL)
      Serial.println(F("Config Data Stream. "));
#endif
      this->HandleConfigDataStream(data);
      break;

    default:
      Serial.print(F("Unknown or not implemented. Dump: 0x"));
      Serial.print(id.command, HEX);
      data.printAsHex();
      Serial.println();
      break;
  }
}

void Station::HandleAccessoryPacket(const RR32Can::Data& data) {
  RR32Can::TurnoutPacket turnoutPacket =
      RR32Can::TurnoutPacket::FromCanPacket(data);
  turnoutPacket.printAll();
}

Engine::Uid_t uidFromData(const uint8_t* ptr) {
  return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
}

Engine* Station::getLocoForData(const RR32Can::Data& data) {
  Engine::Uid_t engineUid = uidFromData(data.data);
  Engine& engine = getEngineControl().getEngine();
  if (!engine.isFullDetailsKnown()) {
    return nullptr;
  }
  if (engine.getUidMasked() != engineUid) {
    return nullptr;
  }

  return &engine;
}

void Station::HandleLocoDirection(const RR32Can::Data& data) {
  if (data.dlc == 5) {
    // response.
    Engine* engine = getLocoForData(data);
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
    Engine* engine = getLocoForData(data);
    if (engine == nullptr) {
      return;
    }
    Engine::Velocity_t velocity = (data.data[4] << 8) | data.data[5];
    engine->setVelocity(velocity);
    ::EngineControl::setEngineVelocity(engine->getVelocity());
  }
}

}  // namespace RR32Can

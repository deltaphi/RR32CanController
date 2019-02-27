#include "RR32Can/Station.h"

#include "RR32Can/utils.h"
#include "RR32Can/Data.h"
#include "RR32Can/Identifier.h"
#include "RR32Can/Handler.h"

#include <Arduino.h>

namespace RR32Can {

void Station::begin(uint16_t stationUUID) {
  senderHash = computeSenderHash(stationUUID);
  expectedConfigData = ConfigDataStreamType::NONE;
  numEnginesKnownByMaster = 0;
}

void Station::HandleConfigDataStream(const RR32Can::Data& data) {
  switch (expectedConfigData) {
    case ConfigDataStreamType::LOKNAMEN:
      // handle incoming engine names
      break;
    case ConfigDataStreamType::LOKINFO:
      // Handle an incoming engine
      break;
    default:
      // Unknown message or no message expected. Ignore.
      Serial.print("Ignoring ConfigData Stream");
  }
}

void Station::RequestEngineList(uint8_t offset) {
    Identifier id{kRequestConfigData, senderHash};

    /* First packet */
    Data data1;
    data1.dlc = 8;
    strncpy(data1.dataAsString(), "loknamen", Data::kDataBufferLength);


    /* Second packet */
    Data data2;
    data2.dlc = snprintf(data2.dataAsString(), Data::kDataBufferLength,"%d %d", offset, kNumEngineNamesDownload);
    if (data2.dlc <= CanDataMaxLength) {
        expectedConfigData = ConfigDataStreamType::LOKNAMEN;
        SendPacket(id, data1);
        SendPacket(id, data2);
    } else {
        // Packet was oversized. Abort the operation.
        return;
    }
}

void Station::RequestEngine(Engine& engine) {
        ;
    if (!engine.isNameKnown() || expectedConfigData != ConfigDataStreamType::NONE) {
        /* Given an empty engine slot or a request is already in progress. Abort. */
        return;
    }

    expectedConfigData = ConfigDataStreamType::LOKINFO;

    Identifier id{kRequestConfigData, senderHash};
    Data data;
    
    /* First packet */
    data.dlc = 8;
    strncpy(data.dataAsString(), "lokinfo ", Data::kDataBufferLength);
    SendPacket(id, data);
    
    /* Second packet */
    strncpy(data.dataAsString(), engine.getName(), CanDataMaxLength);
    data.dlc = strlen(data.dataAsString()); // TODO: Verify of padding with null bytes or spaces is required
    SendPacket(id, data);
}

}  // namespace RR32Can

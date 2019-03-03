#include "RR32Can/Station.h"

#include "RR32Can/EngineBrowser.h"
#include "RR32Can/Handler.h"
#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"
#include "RR32Can/util/utils.h"

#include <Arduino.h>
#include "config.h"

namespace RR32Can {

void Station::begin(uint16_t stationUUID) {
  AbortCurrentConfigRequest();
  senderHash = computeSenderHash(stationUUID);
}

void Station::loop() {
}

void Station::AbortCurrentConfigRequest() {
  if (activeConfigDataConsumer != nullptr) {
    activeConfigDataConsumer->setStreamAborted();
    activeConfigDataConsumer = nullptr;
  }
  expectedConfigData = ConfigDataStreamType::NONE;
  configDataParser.reset();
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
    configDataParser.startStream(&engineBrowser);
    SendPacket(id, data1);
    SendPacket(id, data2);
  } else {
    // Packet was oversized. Abort the operation.
    return;
  }
}

void Station::RequestEngine(Engine& engine) {
  if (!engine.isNameKnown() ||
      expectedConfigData != ConfigDataStreamType::NONE) {
    /* Given an empty engine slot or a request is already in progress. Abort. */
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

}  // namespace RR32Can

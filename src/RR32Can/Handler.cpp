#include "RR32Can/Handler.h"

#include "config.h"

#if (CAN_DRIVER_SJA1000 == STD_ON)
#include <CAN.h>
#endif

#if (CAN_DRIVER_ESP32IDF == STD_ON)
#include "driver/can.h"
#include "driver/gpio.h"
#endif

#include "RR32Can/RR32Can.h"

#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/TurnoutPacket.h"

#include "RR32Can/util/BufferManager.h"
#include "RR32Can/util/TextParser.h"

namespace RR32Can {

void SendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data) {
#if (CAN_DRIVER_ESP32IDF == STD_ON)
  can_message_t message;

  message.flags = CAN_MSG_FLAG_EXTD;
  message.identifier = id.makeIdentifier();
  message.data_length_code = data.dlc;
  memcpy(message.data, data.data, message.data_length_code);
  if (can_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
    printf("Message queued for transmission\n");
  } else {
    printf("Failed to queue message for transmission\n");
  }
#endif

#if (CAN_DRIVER_SJA1000 == STD_ON)
  // Send packet on CAN
  CAN.beginExtendedPacket(id.makeIdentifier());
  for (int i = 0; i < data.dlc; ++i) {
    CAN.write(data.data[i]);
  }
  CAN.endPacket();
#endif
}

} /* namespace RR32Can */

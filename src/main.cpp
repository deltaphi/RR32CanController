#include <Arduino.h>

#include "SPIFFS.h"

#include "config.h"

#if (CAN_DRIVER_SJA1000 == STD_ON)
#include <CAN.h>
#endif

#if (CAN_DRIVER_ESP32IDF == STD_ON)
#include "driver/can.h"
#include "driver/gpio.h"
#endif

#include <RR32Can/RR32Can.h>
#include "RR32Can/Handler.h"

#include "wifiManager.h"

#include <controller/MasterControl.h>

controller::MasterControl masterControl;

void setup() {
  // Start serial and wait for its initialization
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("CAN Turnout Control");

#if (CAN_DRIVER_ESP32IDF == STD_ON)
  // Initialize configuration structures using macro initializers
  can_general_config_t g_config = {
      .mode = CAN_MODE_NORMAL,
      .tx_io = CAN_TX_PIN,
      .rx_io = CAN_RX_PIN,
      .clkout_io = GPIO_NUM_MAX,
      .bus_off_io = GPIO_NUM_MAX,
      .tx_queue_len = 5,
      .rx_queue_len = 100,
      .alerts_enabled = CAN_ALERT_NONE,
      .clkout_divider = 0,
  };
  can_timing_config_t t_config = CAN_TIMING_CONFIG_250KBITS();
  can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();

  // Install CAN driver
  if (can_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    printf("Driver installed\n");
  } else {
    printf("Failed to install driver\n");
    return;
  }

  // Start CAN driver
  if (can_start() == ESP_OK) {
    printf("Driver started\n");
  } else {
    printf("Failed to start driver\n");
    return;
  }
#endif

  // start the CAN bus at 250 kbps
#if (CAN_DRIVER_SJA1000 == STD_ON)
  if (!CAN.begin(250E3)) {
    Serial.println("Starting CAN failed!");
    // Go into an endless loop if this failed.
    // TODO: Use the on-board LED of the esp32 to signal a failure
    while (1)
      ;
  }
#endif

  // Open SPIFFS to be able to store settings
  if (SPIFFS.begin(true)) {
    printf("SPIFFS mount succeeded.\n");
  } else {
    printf("SPIFFS mount failed.\n");
  }

  setupWifi();

  masterControl.begin();

  RR32Can::RR32Can.begin(RR32CanUUID, masterControl);
}

void CanInputLoop(void);

void loop() {
  RR32Can::RR32Can.loop();

  masterControl.loop();

  CanInputLoop();

  WifiInputLoop();
}

#if (CAN_DRIVER_ESP32IDF == STD_ON)
#define IS_EXTENDED (message.flags && CAN_MSG_FLAG_EXTD)
#define IS_RTR (message.flags && CAN_MSG_FLAG_RTR)
#endif
#if (CAN_DRIVER_SJA1000 == STD_ON)
#define IS_EXTENDED (CAN.packetExtended())
#define IS_RTR (CAN.packetRtr())
#endif

void CanInputLoop(void) {
// Check for new message
#if (CAN_DRIVER_ESP32IDF == STD_ON)
  can_status_info_t status_info;
  if (can_get_status_info(&status_info) != ESP_OK) {
    printf("Failed to fetch status info\n");
    return;
  } else if (status_info.msgs_to_rx > 0) {
#if (LOG_CAN_IN_MSG == STD_ON)
    printf("RMC: %i, Missed: %i, ", status_info.msgs_to_rx,
           status_info.rx_missed_count);
#endif
  } else {
    // nohting to do
    return;
  }
#endif

#if (CAN_DRIVER_SJA1000 == STD_ON)
  // Process CAN Frames
  if (CAN.getRmc() > 0) {
#if (LOG_CAN_IN_MSG == STD_ON)
    Serial.print("RMC: ");
    Serial.print(CAN.getRmc(), DEC);
    Serial.print(", ");
#endif
  } else {
    return;
  }
#endif

  // Messages present, receive

#if (CAN_DRIVER_ESP32IDF == STD_ON)
  can_message_t message;
  memset(message.data, 0, CAN_MAX_DATA_LEN);
  if (can_receive(&message, pdMS_TO_TICKS(10000)) != ESP_OK) {
    printf("Failed to receive message\n");
    return;
  }
#endif

#if (CAN_DRIVER_SJA1000 == STD_ON)
  int packetSize = CAN.parsePacket();
  long packetId = CAN.packetId(); /* long is 32 bit */
#endif

#if (LOG_CAN_IN_MSG == STD_ON)
  Serial.print(F("Received frame ("));
  if (IS_EXTENDED) {
    if (IS_RTR) {
      Serial.print(F("extended, remote)."));
    } else {
      Serial.print(F("extended, data).  "));
    }
  } else {
    if (IS_RTR) {
      Serial.print(F("regular,  remote). "));
    } else {
      Serial.print(F("regular,  data).   "));
    }
  }

  Serial.print(F(" ID 0x"));
#if (CAN_DRIVER_SJA1000 == STD_ON)
  Serial.print(packetId, HEX);
#endif
#if (CAN_DRIVER_ESP32IDF == STD_ON)
  Serial.print(message.identifier, HEX);
#endif

  Serial.print(" Length: ");
#if (CAN_DRIVER_SJA1000 == STD_ON)
  if (CAN.packetRtr()) {
    Serial.print(CAN.packetDlc());
  } else {
    Serial.print(packetSize);
  }
#endif
#if (CAN_DRIVER_ESP32IDF == STD_ON)
  Serial.print(message.data_length_code);
#endif

  Serial.print(". Data Bytes (HEX):");
#if (CAN_DRIVER_SJA1000 == STD_ON)
  uint8_t* buf = CAN.getRxBuf();
#endif

#if (CAN_DRIVER_ESP32IDF == STD_ON)
  uint8_t* buf = message.data;
#endif
  for (uint8_t i = 0; i < 8; ++i) {
    Serial.print(" ");
    Serial.print(buf[i], HEX);
  }
  Serial.println();
#endif

  // Read the data bytes into a local buffer.
  // Maerklin does not use remote frames, so we don't care about them.
  RR32Can::Data maerklinData;
  maerklinData.reset();
#if (CAN_DRIVER_SJA1000 == STD_ON)
  maerklinData.dlc = packetSize;
  CAN.readBytes(maerklinData.data, 8);
  RR32Can::Identifier maerklinIdentifier =
      RR32Can::Identifier::GetIdentifier(packetId);
#endif

#if (CAN_DRIVER_ESP32IDF == STD_ON)
  maerklinData.dlc = message.data_length_code;
  memcpy(maerklinData.data, message.data, message.data_length_code);
  RR32Can::Identifier maerklinIdentifier =
      RR32Can::Identifier::GetIdentifier(message.identifier);
#endif

  RR32Can::RR32Can.HandlePacket(maerklinIdentifier, maerklinData);
}

namespace RR32Can {

void SendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data) {
  if (!wifiConnected) {
    Serial.println("Wifi not connected. Not sending packet.");
    return;
  }

  Serial.println("Sending packet via WiFi.");
  // Send packet on WIFI
  udpSendSocket.beginPacket(canGwAddress, canGwPort);
  uint32_t canId = id.makeIdentifier();
  udpSendSocket.write(canId >> 24);
  udpSendSocket.write(canId >> 16);
  udpSendSocket.write(canId >> 8);
  udpSendSocket.write(canId);
  udpSendSocket.write(data.dlc);
  udpSendSocket.write(data.data, 8);
  udpSendSocket.endPacket();
}

} /* namespace RR32Can */
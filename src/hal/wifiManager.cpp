#include "wifiManager.h"

#include "RR32Can/RR32Can.h"

#include "config.h"

boolean wifiConnected = false;
WiFiUDP udpSendSocket;
WiFiClient tcpSocket;

void wifiEventHandler(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_AP_STADISCONNECTED:
    case SYSTEM_EVENT_STA_LOST_IP:
      wifiConnected = false;
      Serial.println("Wifi disconnected.");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
    case SYSTEM_EVENT_STA_GOT_IP: {
      IPAddress localIP = WiFi.localIP();
      IPAddress subnetMask = WiFi.subnetMask();

      Serial.print("Wifi connected. IP: ");
      localIP.printTo(Serial);
      Serial.print("/");
      subnetMask.printTo(Serial);

      uint32_t subnetBits = subnetMask;
      // Invert the subnetBits
      subnetBits = ~subnetBits;
      // Set the inverted bits in the IP address
      uint32_t broadcastBits = localIP;
      broadcastBits |= subnetBits;
      IPAddress broadcastAddress{broadcastBits};

      Serial.print("/");
      broadcastAddress.printTo(Serial);

      Serial.println();

      // UDP port for config data communication
      udpSendSocket.begin(canBroadcastPort + 5);
      // TCP port for most communication
      tcpSocket.connect(canGwAddress, canBroadcastPort + 1);
      wifiConnected = true;
      break;
    }
    case SYSTEM_EVENT_WIFI_READY:
      Serial.println("Wifi on");
      break;
    case SYSTEM_EVENT_STA_START:
      Serial.println("Wifi: Starting STA mode");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Wifi: STA Disconnected. Connection failed.");
      wifiConnected = false;
      break;
    default:
      Serial.print("Wifi event: ");
      Serial.println(event);
      // Don't care about other events
      break;
  }
}

void setupWifi() {
  WiFi.disconnect(false);
  WiFi.onEvent(wifiEventHandler);
}

void startWifi() {
  Serial.println("Starting Wifi. Waiting for connection.");
  WiFi.begin(networkName, networkPswd);
  WiFi.setAutoReconnect(true);
}

void stopWifi() {
  Serial.println("Stopping Wifi.");
  tcpSocket.stop();
  WiFi.setAutoReconnect(false);
  WiFi.disconnect(false);
}

bool isWifiAvailable() { return wifiConnected; }

void WifiInputLoop() {
  if (wifiConnected) {
    if (tcpSocket.available()) {
      // interpret as a CAN packet
      uint8_t identifierBits[] = {0, 0, 0, 0};

      tcpSocket.readBytes(identifierBits, 4);
      RR32Can::CanFrame frame;
      frame.id = RR32Can::Identifier::GetIdentifier(identifierBits);

      frame.data.reset();
      frame.data.dlc = tcpSocket.read();
      tcpSocket.readBytes(frame.data.data, RR32Can::CanDataMaxLength);

#if (LOG_CAN_IN_MSG == STD_ON)
      printf("Wifi Packet: ");
      for (int i = 0; i < 4; ++i) {
        printf("%#02x ", identifierBits[i]);
      }
      data.printAsHex();
      printf("\n");
#endif

      RR32Can::RR32Can.HandlePacket(frame);
    }
  }
}

void WiFiSendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data) {
  if (id.getCommand() == RR32Can::Command::REQUEST_CONFIG_DATA) {
    // Send configuration requests via UDP, as can2lan mangles them on TCP.
    WiFiSendPacketUDP(id, data);
  } else {
    WiFiSendPacketTCP(id, data);
  }
}

void WiFiSendPacketUDP(const RR32Can::Identifier& id, const RR32Can::Data& data) {
  if (!wifiConnected) {
    printf("Wifi not connected. Not sending packet.\n");
    return;
  }

#if (LOG_CAN_OUT_MSG == STD_ON)
  printf("Sending packet via UDP.\n");
#endif
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

void WiFiSendPacketTCP(const RR32Can::Identifier& id, const RR32Can::Data& data) {
  if (!wifiConnected) {
    Serial.println("Wifi not connected. Not sending packet.");
    return;
  }

#if (LOG_CAN_OUT_MSG == STD_ON)
  printf("Sending packet via TCP:");
#endif
  // Send packet on WIFI
  uint32_t canId = id.makeIdentifier();
  tcpSocket.flush();

  uint8_t outBuf[13];

  outBuf[0] = canId >> 24;
  outBuf[1] = canId >> 16;
  outBuf[2] = canId >> 8;
  outBuf[3] = canId;
  outBuf[4] = data.dlc;

  outBuf[5] = data.data[0];
  outBuf[6] = data.data[1];
  outBuf[7] = data.data[2];
  outBuf[8] = data.data[3];
  outBuf[9] = data.data[4];
  outBuf[10] = data.data[5];
  outBuf[11] = data.data[6];
  outBuf[12] = data.data[7];

#if (LOG_CAN_OUT_MSG == STD_ON)
  for (int i = 0; i < 13; ++i) {
    printf(" %#02x", outBuf[i]);
  }
  printf(".\n");
#endif

  tcpSocket.write(outBuf, 13);
}
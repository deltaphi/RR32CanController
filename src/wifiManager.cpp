#include "wifiManager.h"

#include "MaerklinCan/Data.h"
#include "MaerklinCan/Identifier.h"
#include "MaerklinCan/handler.h"

#include "config.h"

boolean wifiConnected = false;
WiFiUDP udpSendSocket;
WiFiUDP udpBroadcastSocket;

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

      udpSendSocket.begin(canBroadcastPort +
                          5);  // Listen on the broadcast port
      udpBroadcastSocket.begin(broadcastAddress, canBroadcastPort);
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
      break;
    default:
      Serial.print("Wifi event: ");
      Serial.println(event);
      // Don't care about other events
      break;
  }
}

void setupWifi() {
  WiFi.disconnect(true);
  WiFi.onEvent(wifiEventHandler);
  Serial.println("Starting Wifi. Waiting for connection.");
  WiFi.begin(networkName, networkPswd);
  WiFi.setAutoReconnect(true);
}

void WifiInputLoop() {
  if (wifiConnected) {
    int packetSize = udpBroadcastSocket.parsePacket();
    if (packetSize != 0) {
      if (packetSize == 13) {
        // interpret as a CAN packet
        uint8_t identifierBits[] = {0, 0, 0, 0};

        udpBroadcastSocket.readBytes(identifierBits, 4);
        MaerklinCan::Identifier id =
            MaerklinCan::Identifier::GetIdentifier(identifierBits);

        MaerklinCan::Data data;
        data.dlc = udpBroadcastSocket.read();
        udpBroadcastSocket.readBytes(data.data, MaerklinCan::CanDataMaxLength);

#if (LOG_CAN_IN_MSG == STD_ON)
        for (int i = 0; i < 4; ++i) {
          Serial.print(identifierBits[i], HEX);
          Serial.print(" ");
        }
        data.printAsHex();
        Serial.println();
#endif

        MaerklinCan::HandlePacket(id, data);
      }

    } else {
#if (LOG_CAN_IN_MSG == STD_ON)
      // Log as arbitrary packet
      Serial.print("Received a packet. Size: ");
      Serial.println(packetSize, DEC);
#endif
    }
    udpBroadcastSocket.flush();  // discard the packet
  }
}
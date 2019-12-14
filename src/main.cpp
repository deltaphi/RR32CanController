#include <Arduino.h>

#include "SPIFFS.h"

#include "config.h"

#include "canManager.h"

#include <RR32Can/RR32Can.h>
#include "RR32Can/Handler.h"

#include "Forwarder.h"
#include "wifiManager.h"


#include <controller/MasterControl.h>

controller::MasterControl masterControl;

canManager canMgr;

model::Settings::CommunicationChannel_t activeCommunicationChannel;

void activateCommunicationChannel(
    model::Settings::CommunicationChannel_t channel);

void setup() {
  // Start serial and wait for its initialization
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("CAN Turnout Control");

  // Open SPIFFS to be able to store settings
  if (SPIFFS.begin(true)) {
    printf("SPIFFS mount succeeded.\n");
  } else {
    printf("SPIFFS mount failed.\n");
  }

  masterControl.begin();

  const model::Settings::Data& userSettings = masterControl.getUserSettings();

  canMgr.begin();
  setupWifi();

  activeCommunicationChannel = userSettings.communicationChannel;
  activateCommunicationChannel(userSettings.communicationChannel);

  RR32Can::RR32Can.begin(RR32CanUUID, masterControl);
}

void loop() {
  RR32Can::RR32Can.loop();

  masterControl.loop();

  WifiInputLoop();

  canMgr.loop();

  model::Settings::CommunicationChannel_t newChannel =
      masterControl.getUserSettings().communicationChannel;

  if (activeCommunicationChannel != newChannel) {
    activateCommunicationChannel(newChannel);
    activeCommunicationChannel = newChannel;
  }

  view::DisplayManager& displayManager = masterControl.getDisplayManager();
  displayManager.setCan(canMgr.isActive());
  displayManager.setWifi(isWifiAvailable());
}

void activateCommunicationChannel(
    model::Settings::CommunicationChannel_t channel) {
  switch (channel) {
    case model::Settings::CommunicationChannel_t::CAN:
      stopWifi();
      canMgr.startCan();
      break;

    case model::Settings::CommunicationChannel_t::WIFI:
      canMgr.startCan(); // Keep CAN active for bridge mode
      startWifi();
      break;
  }
}

void ForwardPacketToWifi(const RR32Can::Identifier& id,
                         const RR32Can::Data& data) {
  printf("Forwarding to WiFi...\n");
  WiFiSendPacket(id, data);
  printf("done\n");
}

void ForwardPacketToCAN(const RR32Can::Identifier& id,
                        const RR32Can::Data& data) {
  printf("Forwarding to CAN...\n");
  canMgr.SendPacket(id, data);
  printf("done\n");
}

namespace RR32Can {

/**
 * \brief Send an arbitrary packet via CAN
 */
void SendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data) {
  if (activeCommunicationChannel ==
          model::Settings::CommunicationChannel_t::CAN &&
      canMgr.isActive()) {
    canMgr.SendPacket(id, data);
  } else if (activeCommunicationChannel ==
                 model::Settings::CommunicationChannel_t::WIFI &&
             isWifiAvailable()) {
    WiFiSendPacket(id, data);
  } else {
    printf("No active communication channels available.\n");
  }
}  // namespace RR32Can

} /* namespace RR32Can */

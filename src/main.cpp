#include <Arduino.h>

#include "SPIFFS.h"

#include "config.h"

#include "canManager.h"

#include <RR32Can/RR32Can.h>
#include "RR32Can/Handler.h"

#include "ConsoleManager.h"
#include "wifiManager.h"

#include <controller/MasterControl.h>

controller::MasterControl masterControl;

#if (CAN_ENABLED == STD_ON)
canManager canMgr;
#endif

ConsoleManager consoleMgr;

model::Settings::CommunicationChannel_t activeCommunicationChannel;

void activateCommunicationChannel(
    model::Settings::CommunicationChannel_t channel);

void setup() {
  // Start serial and wait for its initialization
  consoleMgr.initialize_console();

  printf("\n###################\nCAN Turnout Control\n###################\n");

  // Open SPIFFS to be able to store settings
  if (SPIFFS.begin(true)) {
    printf("SPIFFS mount succeeded.\n");
  } else {
    printf("SPIFFS mount failed.\n");
  }

  masterControl.begin();

  const model::Settings::Data& userSettings = masterControl.getUserSettings();

#if (CAN_ENABLED == STD_ON)
  canMgr.begin();
#endif
  setupWifi();

  activeCommunicationChannel = userSettings.communicationChannel;
  activateCommunicationChannel(userSettings.communicationChannel);

  RR32Can::RR32Can.begin(RR32CanUUID, masterControl);

  consoleMgr.setupCommands(masterControl.getActionListProcessor());

  consoleMgr.StartTask();
}

void loop() {
  RR32Can::RR32Can.loop();

  masterControl.loop();

  WifiInputLoop();

#if (CAN_ENABLED == STD_ON)
  canMgr.loop();
#endif

  model::Settings::CommunicationChannel_t newChannel =
      masterControl.getUserSettings().communicationChannel;

  if (activeCommunicationChannel != newChannel) {
    activateCommunicationChannel(newChannel);
    activeCommunicationChannel = newChannel;
  }

  view::DisplayManager& displayManager = masterControl.getDisplayManager();
#if (CAN_ENABLED == STD_ON)
  displayManager.setCan(canMgr.isActive());
#else
  displayManager.setCan(false);
#endif

  displayManager.setWifi(isWifiAvailable());
}

void activateCommunicationChannel(
    model::Settings::CommunicationChannel_t channel) {
  switch (channel) {
#if (CAN_ENABLED == STD_ON)
    case model::Settings::CommunicationChannel_t::CAN:
      stopWifi();
      canMgr.startCan();
      break;
#endif

    case model::Settings::CommunicationChannel_t::WIFI:
#if (CAN_ENABLED == STD_ON)
      canMgr.stopCan();
#endif
      startWifi();
      break;
  }
}

namespace RR32Can {

/**
 * \brief Send an arbitrary packet via CAN
 */
void SendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data) {
#if (CAN_ENABLED == STD_ON)
  if (activeCommunicationChannel ==
          model::Settings::CommunicationChannel_t::CAN &&
      canMgr.isActive()) {
    canMgr.SendPacket(id, data);
  } else 
#endif
  if (activeCommunicationChannel ==
                 model::Settings::CommunicationChannel_t::WIFI &&
             isWifiAvailable()) {
    WiFiSendPacket(id, data);
  } else {
    printf("No active communication channels available.\n");
  }
}  // namespace RR32Can

} /* namespace RR32Can */

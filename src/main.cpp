#include <Arduino.h>

#include "SPIFFS.h"

#include "config.h"

#include "hal/canManager.h"

#include <RR32Can/RR32Can.h>
#include "RR32Can/Handler.h"

#include "hal/ConsoleManager.h"
#include "hal/DisplayDriver.h"
#include "hal/wifiManager.h"
#include "hal/storage/Settings.h"


#include <controller/MasterControl.h>

controller::MasterControl masterControl;

canManager canMgr;

ConsoleManager consoleMgr;

hal::DisplayDriver displayDriver;

application::model::Settings::CommunicationChannel_t activeCommunicationChannel;

hal::storage::Settings settingsStorage;

void activateCommunicationChannel(
    application::model::Settings::CommunicationChannel_t channel);

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

  masterControl.begin(settingsStorage);
  displayDriver.begin();

  const application::model::Settings& userSettings =
      masterControl.getUserSettings();

  canMgr.begin();
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

  canMgr.loop();

  application::model::Settings::CommunicationChannel_t newChannel =
      masterControl.getUserSettings().communicationChannel;

  if (activeCommunicationChannel != newChannel) {
    activateCommunicationChannel(newChannel);
    activeCommunicationChannel = newChannel;
  }

  auto& displayModel = masterControl.getDisplayModel();
  displayModel.setCan(canMgr.isActive());
  displayModel.setWifi(isWifiAvailable());
  displayDriver.loop(displayModel);
}

void activateCommunicationChannel(
    application::model::Settings::CommunicationChannel_t channel) {
  switch (channel) {
    case application::model::Settings::CommunicationChannel_t::CAN:
      stopWifi();
      canMgr.startCan();
      break;

    case application::model::Settings::CommunicationChannel_t::WIFI:
      canMgr.stopCan();
      startWifi();
      break;
  }
}

namespace RR32Can {

/**
 * \brief Send an arbitrary packet via CAN
 */
void SendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data) {
  if (activeCommunicationChannel ==
          application::model::Settings::CommunicationChannel_t::CAN &&
      canMgr.isActive()) {
    canMgr.SendPacket(id, data);
  } else if (activeCommunicationChannel ==
                 application::model::Settings::CommunicationChannel_t::WIFI &&
             isWifiAvailable()) {
    WiFiSendPacket(id, data);
  } else {
    printf("No active communication channels available.\n");
  }
}  // namespace RR32Can

} /* namespace RR32Can */

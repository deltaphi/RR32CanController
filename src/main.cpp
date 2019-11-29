#include <Arduino.h>

#include "SPIFFS.h"

#include "config.h"

#include "canManager.h"

#include <RR32Can/RR32Can.h>
#include "RR32Can/Handler.h"

#include "wifiManager.h"

#include <controller/MasterControl.h>

controller::MasterControl masterControl;

canManager canMgr;

void setup() {
  // Start serial and wait for its initialization
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("CAN Turnout Control");

  canMgr.begin();

  canMgr.startCan();

  // Open SPIFFS to be able to store settings
  if (SPIFFS.begin(true)) {
    printf("SPIFFS mount succeeded.\n");
  } else {
    printf("SPIFFS mount failed.\n");
  }

  setupWifi();

  masterControl.begin();

  masterControl.getDisplayManager().setWifi(false);

  RR32Can::RR32Can.begin(RR32CanUUID, masterControl);
}

void loop() {
  RR32Can::RR32Can.loop();

  masterControl.loop();

  WifiInputLoop();

  canMgr.loop();

  masterControl.getDisplayManager().setCan(canMgr.isActive());
}

namespace RR32Can {

/**
 * \brief Send an arbitrary packet via CAN
 */
void SendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data) {
  if (canMgr.isActive()) {
    canMgr.SendPacket(id, data);
  } else {
    printf("No active communicatoin channels available.\n");
  }
}

} /* namespace RR32Can */

#ifndef __WIFIMANAGER_H__
#define __WIFIMANAGER_H__

#include <WiFi.h>
#include <WiFiUdp.h>

#include "wificonfig.h"

#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"

// Are we currently connected?
extern boolean wifiConnected;

// The udp library class
extern WiFiUDP udpSendSocket;
extern WiFiUDP udpBroadcastSocket;

void wifiEventHandler(WiFiEvent_t event);
void setupWifi();
void startWifi();
void stopWifi();

void WifiInputLoop(void);

void WiFiSendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data);

bool isWifiAvailable();


#endif
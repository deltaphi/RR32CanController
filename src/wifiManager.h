#ifndef __WIFIMANAGER_H__
#define __WIFIMANAGER_H__

#include <WiFi.h>
#include <WiFiUdp.h>

#include "wificonfig.h"

// Are we currently connected?
extern boolean wifiConnected;

// The udp library class
extern WiFiUDP udpSendSocket;
extern WiFiUDP udpBroadcastSocket;

void wifiEventHandler(WiFiEvent_t event);
void setupWifi();
void WifiInputLoop(void);

#endif
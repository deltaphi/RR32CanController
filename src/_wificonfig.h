#ifndef __WIFICONFIG_H__
#define __WIFICONFIG_H__

/*
 * This is a template file. Copy to "wificonfig.h" and adjust the values for
 * your network.
 */

// WiFi network name and password:
constexpr const char* networkName = "SSID";
constexpr const char* networkPswd = "WiFiPassword";

// IP address of your CS2
constexpr const char* canGwAddress = "192.168.0.15";

// UDP Port on which a CS2 listens for packets
constexpr const int canGwPort = 15731;

// UDP Port on which a CS2 sends broadcast packets
constexpr const int canBroadcastPort = 15730;

#endif
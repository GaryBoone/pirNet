// Copyright 2016 Gary Boone

#ifndef SRC_WIFIMGR_H_
#define SRC_WIFIMGR_H_

#include <ESP8266WiFi.h>  //  ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#define AP_SSID "PenumbralCogitation"
#define AP_PASS "objectsmean"

class WiFiMgr {
 public:
  void joinNetwork(void);
  IPAddress ipAddress(void) { return WiFi.localIP(); }
  String statusString(void) { return _statusStringForWiFi(WiFi.status()); }
  String modeString(void) { return _modeStringForWiFi(WiFi.getMode()); }

 private:
  static const String _wifi_status[];
  static const String _wifi_mode[];

  String _statusStringForWiFi(wl_status_t status);
  String _modeStringForWiFi(WiFiMode_t mode);
};

#endif  // SRC_WIFIMGR_H_

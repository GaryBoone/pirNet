// Copyright 2016 Gary Boone

#ifndef SRC_OTAUPDATES_H_
#define SRC_OTAUPDATES_H_

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

class OtaMgr {
 public:
  void enableUpdates(void);
  void handleUpdates(void) { ArduinoOTA.handle(); }

 private:
  static String _errorStringForOTA(ota_error_t error);
};

#endif  // SRC_OTAUPDATES_H_

// Copyright 2016 Gary Boone

#include "./wifiMgr.h"
#include <DNSServer.h>  //  Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>  //  Local WebServer used to serve the configuration portal
#include <ESP8266WiFi.h>  //  ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <WiFiManager.h>  //  https://github.com/tzapu/WiFiManager WiFi Configuration Magic

const String WiFiMgr::_wifi_status[] = {
    "Idle",           "No SSID available", "Scan completed", "Connected",
    "Connect failed", "Connection lost",   "Disconnected"};

const String WiFiMgr::_wifi_mode[] = {"Off", "Station", "Access point",
                                      "Access point/Station"};

void WiFiMgr::joinNetwork(void) {
  WiFiManager wifiManager;  // Local variable. Won't need once online.
  // wifiManager.resetSettings();  // RESET CREDENTIALS
  wifiManager.setMinimumSignalQuality(30);
  wifiManager.setDebugOutput(true);
  if (!wifiManager.autoConnect(AP_SSID, AP_PASS)) {
    Serial.println("Connection timeout. Resetting.");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  wifiManager.setDebugOutput(false);
}

String WiFiMgr::_statusStringForWiFi(wl_status_t status) {
  if (status < 0 || status > WL_DISCONNECTED) {
    return "unknown status";
  }
  return _wifi_status[status];
}

String WiFiMgr::_modeStringForWiFi(WiFiMode_t mode) {
  if (mode < 0 || mode > WIFI_AP_STA) {
    return "unknown mode";
  }
  return _wifi_mode[mode];
}

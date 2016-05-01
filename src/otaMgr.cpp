// Copyright 2016 Gary Boone

#include "./otaUpdates.h"

String OtaMgr::_errorStringForOTA(ota_error_t error) {
  static const String ota_error[] = {"Authentication Failed", "Begin Failed",
                                     "Connect Failed", "Receive Failed",
                                     "End Failed"};

  if (error < 0 || error > OTA_END_ERROR) {
    return "unknown error";
  }
  return ota_error[error];
}

void OtaMgr::enableUpdates(void) {
  ArduinoOTA.onStart([]() { Serial.println("OTA update started."); });
  ArduinoOTA.onEnd([]() { Serial.println("\nOTA update ended."); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.print("\nError: ");
    Serial.println(_errorStringForOTA(error).c_str());
  });
  ArduinoOTA.begin();
  Serial.println("OTA updates enabled");
}

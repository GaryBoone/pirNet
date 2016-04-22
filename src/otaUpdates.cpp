// Copyright 2016 Gary Boone

#include <ArduinoOTA.h>
#include "./otaUpdates.h"

String errorStringForOTA(ota_error_t error) {
  static const String ota_error[] = {"Authentication Failed", "Begin Failed",
                                     "Connect Failed", "Receive Failed",
                                     "End Failed"};

  if (error < 0 || error > OTA_END_ERROR) {
    return "unknown error";
  }
  return ota_error[error];
}

void enableOTAUpdates(void) {
  ArduinoOTA.onStart([]() { Serial.println("Start"); });
  ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.print("\nError: ");
    Serial.println(errorStringForOTA(error).c_str());
  });
  ArduinoOTA.begin();
  Serial.println("OTA updates enabled");
}

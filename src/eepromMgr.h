// Copyright 2016 Gary Boone

#ifndef SRC_EEPROMMGR_H_
#define SRC_EEPROMMGR_H_

#include <Arduino.h>
#include <EEPROM.h>
#include "./location.h"


class EEPromMgr {
 public:
  static location_t readLocationFromEEProm(void);
  static String readWiFiSSIDFromEEProm(void);
  static String readWiFiPasswordFromEEProm(void);
  static bool writeWiFiSSIDNoCommit(String ssid);
  static bool writeWiFiPasswordNoCommit(String pass);
  static void eraseEeprom(void);
  static bool writeLocationToEEProm(location_t* loc, uint8_t newFloor,
                                    uint8_t newRoom, uint32_t newColor);

 private:
  static bool writeWifiNoCommit(String str, int epStart, int epEnd);
  enum ROM_LAYOUT {
    EEPROM_SSID_START,
    EEPROM_SSID_END = 31,
    EEPROM_PASS_START,
    EEPROM_PASS_END = 95,
    EEPROM_FLOOR_ADDRESS,  // uint8_t
    EEPROM_ROOM_ADDRESS,   // uint8_t
    EEPROM_COLOR_ADDRESS,  // uint32_t
    EEPROM_SIZE = EEPROM_COLOR_ADDRESS + sizeof(uint32_t)
  };
};

#endif  // SRC_EEPROMMGR_H_

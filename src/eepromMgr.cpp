// Copyright 2016 Gary Boone

#include "./eepromMgr.h"
#include "./location.h"

location_t EEPromMgr::readLocationFromEEProm(void) {
  location_t loc;
  EEPROM.get(EEPROM_FLOOR_ADDRESS, loc.floor);
  EEPROM.get(EEPROM_ROOM_ADDRESS, loc.room);
  EEPROM.get(EEPROM_COLOR_ADDRESS, loc.color);
  return loc;
}


String EEPromMgr::readWiFiSSIDFromEEProm(void) {
  String esid = "";
  for (int i = EEPROM_SSID_START; i <= EEPROM_SSID_END; ++i) {
    char c = EEPROM.read(i);
    if (c == 0) {
      break;
    }
    esid += c;
  }
  return esid;
}

String EEPromMgr::readWiFiPasswordFromEEProm(void) {
  String epass = "";
  for (int i = EEPROM_PASS_START; i <= EEPROM_PASS_END; ++i) {
    char c = EEPROM.read(i);
    if (c == 0) {
      break;
    }
    epass += c;
  }
  return epass;
}

bool EEPromMgr::writeWiFiSSIDNoCommit(String ssid) {
  return writeWifiNoCommit(ssid, EEPROM_SSID_START, EEPROM_SSID_END);
}

bool EEPromMgr::writeWiFiPasswordNoCommit(String pass) {
  return writeWifiNoCommit(pass, EEPROM_PASS_START, EEPROM_PASS_END);
}

bool EEPromMgr::writeWifiNoCommit(String str, int epStart, int epEnd) {
  if (str.length() >
      epEnd - epStart) {  // leave last location for terminating 0
    return false;
  }
  for (int epLoc = epStart; epLoc <= epEnd; ++epLoc) {
    int strLoc = epLoc - epStart;
    byte c = strLoc < str.length() ? str[strLoc] : 0;
    EEPROM.write(epLoc, c);
  }
  return true;
}

void EEPromMgr::eraseEeprom(void) {
  for (int i = 0; i < EEPROM_SIZE; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

bool EEPromMgr::writeLocationToEEProm(location_t* loc, uint8_t newFloor, uint8_t newRoom,
                           uint32_t newColor) {
  bool needToCommit = false;
  if (loc->floor != newFloor) {
    loc->floor = newFloor;
    EEPROM.write(EEPROM_FLOOR_ADDRESS, loc->floor);
    needToCommit = true;
  }
  if (loc->room != newRoom) {
    loc->room = newRoom;
    EEPROM.write(EEPROM_ROOM_ADDRESS, loc->room);
    needToCommit = true;
  }
  if (newColor && (loc->color != newColor)) {
    loc->color = newColor;
    EEPROM.put(EEPROM_COLOR_ADDRESS, loc->color);
    needToCommit = true;
  }
  if (needToCommit) {
    EEPROM.commit();
    return true;
  }
  return false;
}

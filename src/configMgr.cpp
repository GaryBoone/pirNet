// Copyright 2016 Gary Boone

#include "./configMgr.h"
#include <ArduinoJson.h>  // https://github.com/bblanchon/ArduinoJson

template <class T>
int ConfigMgr::read(File f, T* value) const {
  if (!f) {
    return 0;
  }
  byte* p = reinterpret_cast<byte*>(value);
  unsigned int i;
  for (i = 0; i < sizeof(*value); i++) {
    *p++ = f.read();
  }
  return i;
}

template <class T>
int ConfigMgr::write(File f, const T& value) const {
  if (!f) {
    return 0;
  }
  const byte* p = reinterpret_cast<const byte*>(&value);
  unsigned int i;
  for (i = 0; i < sizeof(value); i++) {
    f.write(*p++);
  }
  return i;
}

bool ConfigMgr::readLocation(location_t* loc) const {
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount FS.");
  }

  if (!SPIFFS.exists(_filename)) {
    Serial.println("Configuration file doesn't exist.");
    return false;
  }

  File configFile = SPIFFS.open(_filename, "r");
  if (!configFile) {
    Serial.println("Failed to load configuration file.");
    return false;
  }

  read(configFile, &(loc->floor));
  read(configFile, &(loc->room));
  read(configFile, &(loc->color));

  configFile.close();

  return true;
}

bool ConfigMgr::writeLocation(location_t loc) const {
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount FS.");
  }

  File configFile = SPIFFS.open(_filename, "w");
  if (!configFile) {
    Serial.println("Failed to open configuration file for writing");
    return false;
  }

  write(configFile, loc.floor);
  write(configFile, loc.room);
  write(configFile, loc.color);

  configFile.close();

  return true;
}

// The prior value is updated only if the save succeeds.
save_result_t ConfigMgr::writeLocationIfNew(const location_t& newLoc,
                                            location_t* loc) const {
  if (*loc == newLoc) {
    return NO_CHANGE;
  }
  int bytesWritten = writeLocation(newLoc);
  if (bytesWritten > 0) {
    *loc = newLoc;
    return SAVED;
  }
  return SAVE_FAILED;
}

void ConfigMgr::resetConfig(void) const {
  if (!SPIFFS.remove(_filename)) {
    Serial.println("Failed to remove configuration file.");
  }
  // ESP.reset();
  // wifiManager.resetSettings();  // in WiFiMgr
}

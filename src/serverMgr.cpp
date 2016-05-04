// Copyright 2016 Gary Boone

#include "./serverMgr.h"

ServerMgr::ServerMgr(void) { _server = new ESP8266WebServer(80); }
ServerMgr::~ServerMgr(void) { delete _server; }

void ServerMgr::startConfigServer(const ConfigMgr& configMgr, UdpMgr& udpMgr,
                                  const String software_version,
                                  location_t* loc) {
  _server->on("/", [this, software_version, loc]() {
    String content = "<!DOCTYPE HTML>\r\n<html>ESP8266 at ";
    content += "<p>Software version: ";
    content += software_version;
    content += "<p>";
    content +=
        "</p>Enter values from 0 to 255, inclusive: "
        "</p><form method='get' action='setlocation'><label>Floor:&nbsp;"
        "</label><input name='floor' length=10 value=";
    content += String(loc->floor);
    content +=
        ">&nbsp;&nbsp;"
        "<label>Room:&nbsp;</label><input name='room' length=10 value=";
    content += String(loc->room);
    content += ">&nbsp;&nbsp;";
    content +=
        "<label>Color:&nbsp;</label><input type='color' name='mycolor' value=";
    char str[12];
    snprintf(str, sizeof(str), "#%06x", loc->color);
    content += str;
    content += ">&nbsp;&nbsp;";
    content += "<input type='submit'></form>";
    content += "</html>";

    _server->send(200, "text/html", content);
  });

  _server->on("/setlocation", [this, &configMgr, loc]() {
    uint8_t newFloor = _server->arg("floor").toInt();
    uint8_t newRoom = _server->arg("room").toInt();

    String newColorStr = _server->arg("mycolor");
    uint32_t newColor = 0;
    if (newColorStr.length() > 1) {
      char* pEnd;
      newColor = strtol(newColorStr.c_str() + 1, &pEnd, 16);
    }
    location_t newLoc(newFloor, newRoom, newColor);
    // Save the location.
    save_result_t result = configMgr.writeLocationIfNew(newLoc, loc);
    String content = "<!DOCTYPE HTML>\r\n<HTML>";
    content += "<p>";
    switch (result) {
      case NO_CHANGE:
        Serial.println("/setlocation: no change in location.");
        content += "There was no change in the setup.";
        break;
      case SAVED:
        char str[150];
        snprintf(str, sizeof(str),
                 "Saved the new setup: floor=%u, room=%u, color=0x%08x",
                 loc->floor, loc->room, loc->color);
        Serial.printf("/setlocation: %s\r\n", str);
        content += str;
        break;
      case SAVE_FAILED:
        Serial.println("/setlocation: save failed.");
        content += "There was a problem saving the new setup.";
        break;
    }
    content += "<p></HTML>";
    _server->send(200, "text/html", content);
  });

  _server->on("/reset", [this, &configMgr]() {
    configMgr.resetConfig();
    // wifiManager.resetSettings();  // in WiFiMgr
    Serial.println("Configuration reset.");
    String content = "<!DOCTYPE HTML>\r\n<html>";
    content += "<p>Configuration reset.</p>";
    content += "<p>Reboot device.</p></html>";
    _server->send(200, "text/html", content);
  });

  _server->on("/pingconfigs", [this, &udpMgr]() {
    String msg = "pingconfigs";
    int bytesWritten = udpMgr.sendMessage(msg);
    Serial.printf("> sendMessage wrote [%d]: %s\r\n", bytesWritten, msg.c_str());

    String content = "<!DOCTYPE HTML>\r\n<html>";
    content += "<p>All configs pinged.</p></html>";
    _server->send(200, "text/html", content);
  });

  _server->begin();
  Serial.println("Configuration server started.");
}

void ServerMgr::handleRequests(void) { _server->handleClient(); }

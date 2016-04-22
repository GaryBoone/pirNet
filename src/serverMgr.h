// Copyright 2016 Gary Boone

#ifndef SRC_SERVERMGR_H_
#define SRC_SERVERMGR_H_

#include <ESP8266WiFi.h>       // 1 Must come before ESP8266WebServer.h
#include <ESP8266WebServer.h>  // 2 Must come after ESP8266WiFi.h
#include "./configMgr.h"
#include "./location.h"

class ServerMgr {
 public:
  ServerMgr(void);
  ~ServerMgr(void);
  void startConfigServer(const ConfigMgr& configMgr,
                         const String software_version, location_t* loc);
  void handleRequests(void);

 private:
  ESP8266WebServer* _server;
};

#endif  // SRC_SERVERMGR_H_

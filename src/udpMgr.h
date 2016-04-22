// Copyright 2016 Gary Boone

#ifndef SRC_UDPMGR_H_
#define SRC_UDPMGR_H_

#include <WiFiUdp.h>
#include "./location.h"

class UdpMgr {
 public:
  UdpMgr(const IPAddress broadcastIP, const int localPort);
  void sendUDP(const location_t& loc);
  int localPort(void) { return _udp.localPort(); }
  int remotePort(void) { return _udp.remotePort(); }
  IPAddress remoteIP(void) { return _udp.remoteIP(); }
  int parsePacket(void) { return _udp.parsePacket(); }
  int read(location_t *loc) { return _udp.read(reinterpret_cast<byte*>(loc), sizeof(location_t)); }
  int read(unsigned char* buffer, size_t len) { return _udp.read(buffer, len); }
  int read(char* buffer, size_t len) { return _udp.read(buffer, len); }
  int read(void) { return _udp.read(); }

 private:
  WiFiUDP _udp;
  int _ipBroadcast;
  int _udpLocalPort;
};

#endif  // SRC_UDPMGR_H_

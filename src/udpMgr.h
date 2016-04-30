// Copyright 2016 Gary Boone

#ifndef SRC_UDPMGR_H_
#define SRC_UDPMGR_H_

#include <WiFiUdp.h>
#include "./location.h"


class UdpMgr {
 public:
  typedef void (*callback_t)(void);

  UdpMgr(const IPAddress broadcastIP, const int localPort);
  void attach(int size, callback_t callback);
  void detach(void);
  void reportUDPPacket(int packetSize);
  void readUDPOther(int packetSize);
  void receiveUDP(void);

  void sendUDP(const location_t& loc);
  int localPort(void) { return _udp.localPort(); }
  int remotePort(void) { return _udp.remotePort(); }
  IPAddress remoteIP(void) { return _udp.remoteIP(); }
  int parsePacket(void) { return _udp.parsePacket(); }
  int read(location_t *loc) { return _udp.read(reinterpret_cast<byte*>(loc), sizeof(location_t)); }
  int read(unsigned char* buffer, size_t len) { return _udp.read(buffer, len); }
  
  // TODO(Gary): Remove these.
  // int read(char* buffer, size_t len) { return _udp.read(buffer, len); }
  // int read(void) { return _udp.read(); }

 private:
  WiFiUDP _udp;
  int _ipBroadcast;
  int _udpLocalPort;
  int _callbackOnSize;
  callback_t _callback;
  char _packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
};

#endif  // SRC_UDPMGR_H_

// Copyright 2016 Gary Boone

#ifndef SRC_UDPMGR_H_
#define SRC_UDPMGR_H_

#include <WiFiUdp.h>
#include "./location.h"

class UdpMgr {
 public:
  typedef String (*callback_t)(void);

  UdpMgr(const IPAddress broadcastIP, const int localPort);

  // Callback
  void attach(int size, callback_t callback);
  void detach(void);

  // Read
  String readMessage(void);
  String read(void);
  int read(unsigned char* buffer, size_t len) { return _udp.read(buffer, len); }
  location_t readLocation(void);

  // Write
  void sendLocation(const location_t& loc);
  int sendMessage(const String& msg);

  // Info
  void reportUDPPacket(int packetSize);
  int localPort(void) { return _udp.localPort(); }
  int remotePort(void) { return _udp.remotePort(); }
  IPAddress remoteIP(void) { return _udp.remoteIP(); }

 private:
  WiFiUDP _udp;
  int _ipBroadcast;
  int _udpLocalPort;
  int _callbackOnSize;
  callback_t _callback;
  char _packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer for incoming packet,
};

#endif  // SRC_UDPMGR_H_

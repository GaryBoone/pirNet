// Copyright 2016 Gary Boone

#include "./udpMgr.h"
#include <WiFiUdp.h>

UdpMgr::UdpMgr(const IPAddress broadcastIP, const int localPort) {
  _ipBroadcast = broadcastIP;
  _udpLocalPort = localPort;
  _callbackOnSize = 0;
  _callback = NULL;
  _udp.begin(_udpLocalPort);
}

void UdpMgr::attach(int size, callback_t callback) {
  _callbackOnSize = size;
  _callback = callback;
}

void UdpMgr::detach(void) {
  _callbackOnSize = 0;
  _callback = NULL;
}

// Allow printable characters only.
String UdpMgr::readMessage(void) {
  int bytesRead = _udp.read(_packetBuffer, UDP_TX_PACKET_MAX_SIZE);
  for (int i = 0; i < bytesRead; ++i) {
    char c = _packetBuffer[i];
    if (c != 0 && (c < ' ' || c > '~')) {
      Serial.printf(" readMessage: bad char: %d: '%c'\n\r", c, c);
      return "";
    }
  }
  Serial.printf("< [read: %d] packet: '%s'\n\r", bytesRead, _packetBuffer);
  return _packetBuffer;
}

String UdpMgr::read(void) {
  int packetSize = _udp.parsePacket();
  if (!packetSize) {
    return "";
  }
  reportUDPPacket(packetSize);
  if (packetSize != _callbackOnSize) {
    return readMessage();;
  }
  return _callback();
}

location_t UdpMgr::readLocation(void) {
  location_t l;
  l.floor = _udp.read();
  l.room = _udp.read();
  _udp.read(reinterpret_cast<uint8_t*>(&l.color), sizeof(uint32_t));
  return l;
}

void UdpMgr::sendLocation(const location_t& loc) {
  _udp.beginPacket(_ipBroadcast, _udpLocalPort);
  _udp.write(loc.floor);
  _udp.write(loc.room);
  _udp.write(reinterpret_cast<const uint8_t*>(&loc.color), sizeof(uint32_t));
  _udp.endPacket();
}

int UdpMgr::sendMessage(const String& msg) {
  _udp.beginPacket(_ipBroadcast, _udpLocalPort);
  strncpy(_packetBuffer, msg.c_str(), sizeof(_packetBuffer));
  int bytesWritten = _udp.write(_packetBuffer, msg.length() + 1);
  _udp.endPacket();
  return bytesWritten;
}

void UdpMgr::reportUDPPacket(int packetSize) {
  Serial.printf("< Received %d bytes", packetSize);
  IPAddress r = _udp.remoteIP();
  Serial.printf(" from %d.%d.%d.%d", r[0], r[1], r[2], r[3]);
  Serial.printf(", port %d\n\r", _udp.remotePort());
}



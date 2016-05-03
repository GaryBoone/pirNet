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

void UdpMgr::reportUDPPacket(int packetSize) {
  Serial.printf("< Received %d bytes", packetSize);
  IPAddress r = _udp.remoteIP();
  Serial.printf(" from %d.%d.%d.%d", r[0], r[1], r[2], r[3]);
  Serial.printf(", port %d\n\r", _udp.remotePort());
}

// Allow printable characters only.
String UdpMgr::readUDPOther(int packetSize) {
  if (!packetSize) {
    return "";
  }
  int bytesRead = _udp.read(_packetBuffer, UDP_TX_PACKET_MAX_SIZE);
  for (int i = 0; i < bytesRead; ++i) {
    char c = _packetBuffer[i];
    if (c != 0 && (c < ' ' || c > '~')) {
      Serial.printf(" bad char: %d: '%c'\n\r", c, c);
      return "";
    }
  }
  Serial.printf("< [read: %d] packet: '%s'\n\r", bytesRead, _packetBuffer);
  return _packetBuffer;
}

String UdpMgr::receiveUDP(void) {
  int packetSize = _udp.parsePacket();
  if (!packetSize) {
    return "";
  }
  reportUDPPacket(packetSize);
  if (packetSize != _callbackOnSize) {
    return readUDPOther(packetSize);;
  }
  return _callback();
}

void UdpMgr::sendUDP(const location_t& loc) {
  _udp.beginPacket(_ipBroadcast, _udpLocalPort);
  // strcpy(udpBuffer, "hello testing message");
  // _udp.write(udpBuffer, sizeof(udpBuffer));

  _udp.write(loc.floor);
  _udp.write(loc.room);

  // _udp.write((uint8_t*)&loc.color, sizeof(uint32_t));
  _udp.write((loc.color >> 24) & 0xFF);
  _udp.write((loc.color >> 16) & 0xFF);
  _udp.write((loc.color >> 8) & 0xFF);
  _udp.write(loc.color & 0xFF);

  // If we just write the location_t struct, it sends 8 bytes.
  // _udp.write((byte*)&loc, sizeof(location_t));
  _udp.endPacket();

  // Serial.printf(
  //     "> sending values: floor=%u, room=%u, a=%u, b=%u, c=%u, d=%u\r\n",
  //     loc.floor, loc.room, a, b, c, d);
  Serial.printf("> sending values: floor=%u, room=%u, color=0x%08x\r\n",
                loc.floor, loc.room, loc.color);
}

void UdpMgr::sendUDP(const String& msg) {
  _udp.beginPacket(_ipBroadcast, _udpLocalPort);
  strncpy(_packetBuffer, msg.c_str(), sizeof(_packetBuffer));
  int bytesWritten = _udp.write(_packetBuffer, msg.length() + 1);
  _udp.endPacket();
  Serial.printf("> sendUDP wrote [%u]: %s\r\n", bytesWritten, msg.c_str());
}

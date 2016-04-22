// Copyright 2016 Gary Boone

#include <WiFiUdp.h>
#include "./udpMgr.h"

UdpMgr::UdpMgr(const IPAddress broadcastIP, const int localPort) {
  _ipBroadcast = broadcastIP;
  _udpLocalPort = localPort;
  _udp.begin(_udpLocalPort);
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

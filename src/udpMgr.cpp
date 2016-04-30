// Copyright 2016 Gary Boone

#include <WiFiUdp.h>
#include "./udpMgr.h"

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

void UdpMgr::readUDPOther(int packetSize) {
  if (!packetSize) {
    return;
  }
  Serial.printf("< [%d] Contents: ", packetSize);
  if (packetSize == 1) {
    uint8_t value = _udp.read();
    Serial.println(value);
  } else {
    // Read the packet into packetBufffer.
    _udp.read(_packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.printf("%d", _packetBuffer[0]);
    for (int i = 1; i < packetSize; ++i) {
      Serial.printf(", %d", _packetBuffer[i]);
    }
    Serial.println("");
  }
}

void UdpMgr::receiveUDP(void) {
  int packetSize = _udp.parsePacket();
  if (!packetSize) {
    return;
  }
  reportUDPPacket(packetSize);
  if (packetSize != _callbackOnSize) {
    readUDPOther(packetSize);
    return;
  }
  _callback();
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

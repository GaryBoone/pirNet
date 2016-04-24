// Copyright 2016 Gary Boone

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>       // 1 Must come before ESP8266WebServer.h
#include <ESP8266mDNS.h>
#include <Ticker.h>
#include "./buildVersion.h"
#include "./configMgr.h"
#include "./displayMgr.h"
#include "./location.h"
#include "./otaUpdates.h"
#include "./serverMgr.h"
#include "./wifiMgr.h"
#include "./udpMgr.h"

// Quotes and parentheses must be balanced.
#define MULTILINE_STRING(...) #__VA_ARGS__

const int pirInputPin = 12;
#define NEOPIXEL_ARRAY_PIN 15
const int neopixelBrightness = 50;

const String software_version = buildVersionString(__TIMESTAMP__);


IPAddress ipBroadCast(192, 168, 1, 255);
unsigned int udplocalPort = 2390;  // TODO(Gary): change to 6484
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,

const int LED_BLUE = 2;
const int LED_RED = LED_BUILTIN;

Ticker blueTicker;
const int flipBlueInterval = 200;  // ms

Ticker oledTicker;
const int updateOLEDInterval = 100;  // ms

Ticker pirTicker;
const int updatePIRInterval = 1000;  // ms

Ticker receiveUDPTicker;
const int receiveUDPInterval = 100;  // ms

Ticker testUDPSendTicker;
const int testUDPSendInterval = 2000;  // ms

UdpMgr udpMgr(ipBroadCast, udplocalPort);
WiFiMgr wiFiMgr;
ConfigMgr configMgr;
ServerMgr serverMgr;
DisplayMgr displayMgr(NEOPIXEL_ARRAY_PIN, neopixelBrightness);

location_t loc(1, 1, 0x123456);


// TODO(G): used?
// Return true if any of the first four characters of the given ESID are 0.
bool esidIsBlank(const String& esid) {
  for (int i = 0; i < 4; ++i) {
    if (esid[0] != 0) {
      return false;
    }
  }
  return true;
}


void SetRandomSeed() {
  uint32_t seed;

  // random works best with a seed that can use 31 bits
  // analogRead on a unconnected pin tends toward less than four bits
  seed = analogRead(0);
  delay(1);

  for (int shifts = 3; shifts < 31; shifts += 3) {
    seed ^= analogRead(0) << shifts;
    delay(1);
  }

  // Serial.println(seed);
  randomSeed(seed);
}

// timer function
void flipBlue(void) {
  digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));
}

// timer function
void updatePIR(void) {
  uint8_t val = digitalRead(pirInputPin);
  if (val) {
    displayMgr.writeToDisplay(loc.floor, loc.room, loc.color);
    udpMgr.sendUDP(loc);
  }
}

void reportUDPPacket(int packetSize) {
  Serial.printf("< Received %d bytes", packetSize);
  IPAddress r = udpMgr.remoteIP();
  Serial.printf(" from %d.%d.%d.%d", r[0], r[1], r[2], r[3]);
  Serial.printf(", port %d\n\r", udpMgr.remotePort());
}

void readUDPOther(int packetSize) {
  if (!packetSize) {
    return;
  }
  if (packetSize == 1) {
    uint8_t value = udpMgr.read();
    Serial.print("< 1 Contents: ");
    Serial.println(value);
  } else {
    // read the packet into packetBufffer
    udpMgr.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.print("< [other] Contents: ");
    Serial.printf("%d", packetBuffer[0]);
    for (int i = 1; i < packetSize; ++i) {
      Serial.printf(", %d", packetBuffer[i]);
    }
    Serial.println("");
  }
}

void readUDPFloorRoomColor(int packetSize) {
  if (packetSize != 6) {
    return;
  }

  location_t l;
  udpMgr.read(reinterpret_cast<byte*>(&l), sizeof(location_t));
  Serial.printf("< Received floor=%u, room=%u, value=0x%08x\r\n", l.floor, l.room, l.color);
  displayMgr.writeToDisplay(l.floor, l.room, l.color);
}

// timer function
void updateOLED(void) {
  displayMgr.updateDisplay();
}

// TODO(Gary): need this?
// timer function
void testUDPSend(void) {
  udpMgr.sendUDP(loc);
}

// timer function
void receiveUDP(void) {
  int packetSize = udpMgr.parsePacket();
  if (!packetSize) {
    return;
  }
  reportUDPPacket(packetSize);
  if (packetSize == 6) {
    readUDPFloorRoomColor(packetSize);
    digitalWrite(LED_RED, !digitalRead(LED_RED));
  } else {
    readUDPOther(packetSize);
  }
}

void enableTimers(void) {
  oledTicker.attach_ms(updateOLEDInterval, updateOLED);
  pirTicker.attach_ms(updatePIRInterval, updatePIR);
  blueTicker.attach_ms(flipBlueInterval, flipBlue);
  receiveUDPTicker.attach_ms(receiveUDPInterval, receiveUDP);
  testUDPSendTicker.attach_ms(testUDPSendInterval, testUDPSend);
}

void disableTimers(void) {
  oledTicker.detach();
  pirTicker.detach();
  blueTicker.detach();
  receiveUDPTicker.detach();
  testUDPSendTicker.detach();
}

void setup() {
  SetRandomSeed();
  Serial.begin(115200);
  Serial.printf("Software version: %s\r\n", software_version.c_str());

  wiFiMgr.joinNetwork();
  Serial.print("Local IP: ");
  Serial.println(wiFiMgr.ipAddress());
  Serial.print("Status: ");
  Serial.println(wiFiMgr.statusString());
  Serial.print("Mode: ");
  Serial.println(wiFiMgr.modeString());

  enableTimers();
  enableOTAUpdates();

  bool readLoc = configMgr.readLocation(&loc);
  if (!readLoc) {
    Serial.println("Failed to read location. Using defaults.");
  }
  Serial.printf("ESP8266 Location: floor=%u, room=%u, color=0x%08x\r\n",
                loc.floor, loc.room, loc.color);

  serverMgr.startConfigServer(configMgr, software_version, &loc);

  Serial.print("UDP Local port: ");
  Serial.println(udpMgr.localPort());

  // LEDs
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_BLUE, 0);
  digitalWrite(LED_RED, 0);

  // PIR sensor
  pinMode(pirInputPin, INPUT);  // declare sensor as input
}

void loop() {
  ArduinoOTA.handle();

  serverMgr.handleRequests();

  delay(50);
}

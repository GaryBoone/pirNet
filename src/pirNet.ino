// Copyright 2016 Gary Boone

#include "./buildVersion.h"
#include "./configMgr.h"
#include "./displayMgr.h"
#include "./location.h"
#include "./otaMgr.h"
#include "./serverMgr.h"
#include "./timerMgr.h"
#include "./udpMgr.h"
#include "./wifiMgr.h"

// Pins
const int pirInputPin = 12;
const int neoPixelArrayPin = 15;

// Display
const int neoPixelBrightness = 50;

// UDP
IPAddress ipBroadCast(192, 168, 1, 255);
unsigned int udplocalPort = 6484;

// Timers
const int flipBlueInterval = 200;      // ms
const int updateOLEDInterval = 100;    // ms
const int updatePIRInterval = 1000;    // ms
const int readSensorsInterval = 100;   // ms

// LEDs
const int ledBlue = 2;
const int ledRed = LED_BUILTIN;

// Build version
const String software_version = buildVersionString(__TIMESTAMP__);

// Managers
ConfigMgr configMgr;
WiFiMgr wiFiMgr;
ServerMgr serverMgr;
UdpMgr udpMgr(ipBroadCast, udplocalPort);
OtaMgr otaMgr;
TimerMgr timerMgr;
DisplayMgr displayMgr(neoPixelArrayPin, neoPixelBrightness);

// Location global
location_t loc(1, 1, 0x123456);

// Random works best with a seed that can use 31 bits.
// analogRead on a unconnected pin tends toward less than four bits.
void SetRandomSeed() {
  uint32_t seed = analogRead(0);
  delay(1);
  for (int shifts = 3; shifts < 31; shifts += 3) {
    seed ^= analogRead(0) << shifts;
    delay(1);
  }
  randomSeed(seed);
}

// timer function
void flipBlue(void) { digitalWrite(ledBlue, !digitalRead(ledBlue)); }

// timer function
void updatePIR(void) {
  uint8_t activity = digitalRead(pirInputPin);
  if (activity) {
    displayMgr.writeToDisplay(loc.floor, loc.room, loc.color);
    udpMgr.sendLocation(loc);
    Serial.printf("> sending location: floor=%u, room=%u, color=0x%08x\r\n",
                  loc.floor, loc.room, loc.color);
  }
}

// timer function
void updateOLED(void) { displayMgr.updateDisplay(); }

// timer function
void receiveNetworkSensors(void) {
  String str = udpMgr.read();
  if (str == "pingconfigs") {
    Serial.printf("> sending configs: floor=%u, room=%u, color=0x%08x\r\n",
                  loc.floor, loc.room, loc.color);
    udpMgr.sendLocation(loc);
  }
}

void enableTimers(void) {
  timerMgr.add(updateOLEDInterval, updateOLED);
  timerMgr.add(updatePIRInterval, updatePIR);
  timerMgr.add(flipBlueInterval, flipBlue);
  timerMgr.add(readSensorsInterval, receiveNetworkSensors);
}

// Called on PIR detection.
void readUDPLocation(void) {
  location_t l = udpMgr.readLocation();
  displayMgr.writeToDisplay(l.floor, l.room, l.color);
  Serial.printf("< Received floor=%u, room=%u, value=0x%08x\r\n", l.floor,
                l.room, l.color);
}

void reportStatus(void) {
  Serial.printf("Software version: %s\r\n", software_version.c_str());
  Serial.print("Local IP: ");
  Serial.println(wiFiMgr.ipAddress());
  Serial.print("Status: ");
  Serial.println(wiFiMgr.statusString());
  Serial.print("Mode: ");
  Serial.println(wiFiMgr.modeString());
  Serial.printf("ESP8266 Location: floor=%u, room=%u, color=0x%08x\r\n",
                loc.floor, loc.room, loc.color);
  Serial.print("UDP Local port: ");
  Serial.println(udpMgr.localPort());
}

void setup() {
  SetRandomSeed();
  Serial.begin(115200);

  pinMode(pirInputPin, INPUT);
  pinMode(ledBlue, OUTPUT);
  digitalWrite(ledBlue, 0);
  pinMode(ledRed, OUTPUT);
  digitalWrite(ledRed, 0);

  wiFiMgr.joinNetwork();
  otaMgr.enableUpdates();

  configMgr.readLocation(&loc);

  serverMgr.startConfigServer(configMgr, udpMgr, software_version, &loc);

  enableTimers();

  udpMgr.attach(locationSizeOnWire, []() -> String {
    readUDPLocation();
    digitalWrite(ledRed, !digitalRead(ledRed));
    return "";
  });

  reportStatus();
}

void loop() {
  otaMgr.handleUpdates();

  serverMgr.handleRequests();

  delay(50);
}

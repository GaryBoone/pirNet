// Copyright 2016 Gary Boone

#include <Adafruit_NeoPixel.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>       // 1 Must come before ESP8266WebServer.h
#include <ESP8266mDNS.h>
#include <Ticker.h>
#include "./buildVersion.h"
#include "./configMgr.h"
#include "./location.h"
#include "./otaUpdates.h"
#include "./serverMgr.h"
#include "./wifiMgr.h"
#include "./udpMgr.h"

// Quotes and parentheses must be balanced.
#define MULTILINE_STRING(...) #__VA_ARGS__

#define NEOPIXEL_ARRAY_PIN 15
const int pirInputPin = 12;
#define NUM_NEOPIXELS 32
#define NUM_ROOMS 8
const int neopixelBrightness = 50;

Adafruit_NeoPixel pixels =
    Adafruit_NeoPixel(NUM_NEOPIXELS, NEOPIXEL_ARRAY_PIN, NEO_GRB + NEO_KHZ800);

const String software_version = buildVersionString(__TIMESTAMP__);


IPAddress ipBroadCast(192, 168, 1, 255);
unsigned int udplocalPort = 2390;  // TODO(Gary): change to 6484
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,

const int LED_BLUE = 2;
const int LED_RED = LED_BUILTIN;

uint32_t display[2][NUM_ROOMS];
uint8_t writeDisplay = 0;

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

location_t loc(1, 1, 0x123456);


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


// 0>(0,0)>0                 // 0>(0,0)>0
// 1>(0,1)>4                 // 1>(0,1)>16
// 2>(1,0)>1                 // 2>(1,0)>2
// 3>(1,1)>5                 // 3>(1,1)>18
// 4>(2,0)>2                 // 4>(2,0)>4
// 5>(2,1)>6                 // 5>(2,1)>20
// 6>(3,0)>3                 // 6>(3,0)>6
// 7>(3,1)>7                 // 7>(3,1)>22
// = x + y*4                 // = 2*x + y*16

// (f,r)
// 0>(0,0)>0                 // 0>(0,0)>0
// 1>(0,1)>4                 // 1>(0,1)>16
// 2>(0,2)>1                 // 2>(0,2)>2
// 3>(0,3)>5                 // 3>(0,3)>18
// 4>(1,0)>2                 // 4>(1,0)>4
// 5>(1,1)>6                 // 5>(1,1)>20
// 6>(1,2)>3                 // 6>(1,2)>6
// 7>(1,3)>7                 // 7>(1,3)>22
// = x*2 + (y%2)*4+y/2       // = x*4 + (y%2)*16+(y/2)*2

void setDisplay(uint8_t flr, uint8_t room, uint32_t color) {
  // display buffer:
  // uint8_t loc = (flr - 1) * 2  + ((room - 1) % 2) * 4 + (room - 1) / 2;  //
  // floor/room are 1-based.
  // display[loc] = pixelValue;

  uint8_t pixel = (flr - 1) * 4 + ((room - 1) % 2) * 16 +
                  ((room - 1) / 2) * 2;  // floor/room are 1-based.
  // uint8_t pixel = (flr - 1) * 2 + (room - 1) * 16;  // floor/room are
  // 1-based.
  pixels.setPixelColor(pixel, color);
  pixels.setPixelColor(pixel + 1, color);
  pixels.setPixelColor(pixel + 8, color);
  pixels.setPixelColor(pixel + 9, color);
  // pixels.show();
}

void writeToDisplay(uint8_t flr, uint8_t room, uint32_t color) {
  if (flr < 1 || flr > 2 || room < 1 || room > 4) {
    return;
  }
  display[writeDisplay][(flr - 1) * 4 + (room - 1)] = color;
}

void flipBlue(void) {
  digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));
}

void updateOLED(void) {
  writeDisplay = !writeDisplay;

  // Fade all pixels.
  for (int i = 0; i < NUM_NEOPIXELS; i++) {
    uint32_t color = pixels.getPixelColor(i);
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    r = r > 0 ? r - 1 : 0;
    g = g > 0 ? g - 1 : 0;
    b = b > 0 ? b - 1 : 0;
    color = pixels.Color(r, g, b);
    pixels.setPixelColor(i, color);
  }

  // Write the display state values to the OLED board.
  uint8_t readDisplay = !writeDisplay;
  for (int i = 0; i < 8; ++i)  {
    uint32_t color = display[readDisplay][i];
    if (color != 0) {  // Don't write zeros, which will erase fading non-zero colors.
      setDisplay((i / 4) + 1, (i % 4) + 1, color);
      Serial.printf("- display color for pixel %d = %08x\r\n", i, color);
    }
  }
  pixels.show();

  // Clear the read display.
  memset(display[readDisplay], 0, NUM_ROOMS * sizeof(uint32_t));
}

void updatePIR(void) {
  uint8_t val = digitalRead(pirInputPin);
  if (val) {
    writeToDisplay(loc.floor, loc.room, loc.color);
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
  writeToDisplay(l.floor, l.room, l.color);
}

// TODO(Gary): need this?
void testUDPSend(void) {
  udpMgr.sendUDP(loc);
}

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

  // TODO(Gary): Move these.
  // set UDP port for listen
  Serial.print("UDP Local port: ");
  Serial.println(udpMgr.localPort());

  // LEDs
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_BLUE, 0);
  digitalWrite(LED_RED, 0);

  // Neopixel array
  pinMode(NEOPIXEL_ARRAY_PIN, OUTPUT);
  pixels.setBrightness(neopixelBrightness);
  pixels.begin();

  memset(display, 0, 2 * NUM_ROOMS * sizeof(uint32_t));

  // PIR sensor
  pinMode(pirInputPin, INPUT);  // declare sensor as input
}

void loop() {
  ArduinoOTA.handle();

  serverMgr.handleRequests();

  delay(50);
}

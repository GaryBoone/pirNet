// Copyright 2016 Gary Boone

#include "./displayMGR.h"

displayMgr::DisplayMgr(int pin, int brightness) {
  pinMode(pin, OUTPUT);
  Adafruit_NeoPixel _pixels =
      new Adafruit_NeoPixel(NUM_NEOPIXELS, pin, NEO_GRB + NEO_KHZ800);

  memset(display, 0, 2 * NUM_ROOMS * sizeof(uint32_t));

  _pixels.setBrightness(brightness);
  _pixels.begin();
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

void DisplayMgr::setDisplay(uint8_t flr, uint8_t room, uint32_t color) {
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

void DisplayMgr::writeToDisplay(uint8_t flr, uint8_t room, uint32_t color) {
  if (flr < 1 || flr > 2 || room < 1 || room > 4) {
    return;
  }
  display[writeDisplay][(flr - 1) * 4 + (room - 1)] = color;
}

void DisplayMgr::updateOLED(void) {
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

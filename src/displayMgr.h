// Copyright 2016 Gary Boone

#ifndef SRC_DISPLAYMGR_H_
#define SRC_DISPLAYMGR_H_

#include <Adafruit_NeoPixel.h>

#define NUM_NEOPIXELS 32
#define NUM_ROOMS 8

class DisplayMg {
 public:
  Display(int pin, int brightness);
  ~Display(void) { delete _pixels; }
  void setDisplay(uint8_t flr, uint8_t room, uint32_t color);
  void writeToDisplay(uint8_t flr, uint8_t room, uint32_t color);
  void updateOLED(void);

 private:
  Adafruit_NeoPixel* _pixels;
  uint32_t display[2][NUM_ROOMS];
  uint8_t writeDisplay = 0;

};

#endif  // SRC_DISPLAYMGR_H_

// Copyright 2016 Gary Boone

#ifndef SRC_DISPLAYMGR_H_
#define SRC_DISPLAYMGR_H_

#include <Adafruit_NeoPixel.h>

#define NUM_NEOPIXELS 32
#define NUM_DISPLAY_PAGES 2  // active, background so updates are no on active display
#define NUM_ROOMS 8

class DisplayMgr {
 public:
  DisplayMgr(int pin, int brightness);
  ~DisplayMgr(void) { delete _pixels; }
  void setDisplay(uint8_t flr, uint8_t room, uint32_t color);
  void writeToDisplay(uint8_t flr, uint8_t room, uint32_t color);
  void updateDisplay(void);


 private:
  Adafruit_NeoPixel* _pixels;
  uint32_t _display[NUM_DISPLAY_PAGES][NUM_ROOMS];
  uint8_t _writeDisplay = 0;

  void switchBuffers(void) { _writeDisplay = !_writeDisplay; }
  void fade(void);
  void update(void);
  void show(void);
};

#endif  // SRC_DISPLAYMGR_H_

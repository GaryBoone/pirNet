// Copyright 2016 Gary Boone

#ifndef SRC_LOCATION_H_
#define SRC_LOCATION_H_

#include <Arduino.h>

struct location_t {
  uint8_t floor = 0;
  uint8_t room = 0;
  uint32_t color = 0;
  location_t(void) {}
  location_t(uint8_t f, uint8_t r, uint32_t c) : floor(f), room(r), color(c) {}
  bool operator==(const location_t& rhs) {
    return floor == rhs.floor && room == rhs.room && color == rhs.color;
  }
  bool operator!=(const location_t& rhs) {
    return floor != rhs.floor || room != rhs.room || color != rhs.color;
  }
};

#endif  // SRC_LOCATION_H_

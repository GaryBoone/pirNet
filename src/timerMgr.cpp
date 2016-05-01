// Copyright 2016 Gary Boone

#include "./timerMgr.h"

TimerMgr::~TimerMgr(void) {
  for (int i = 0; i < _timers.size(); ++i) {
    _timers[i]->detach();
    delete _timers[i];
  }
}

void TimerMgr::add(int milliseconds, Ticker::callback_t callback) {
  int newIndex = _timers.size();
  _timers.push_back(new Ticker());
  _timers[newIndex]->attach(milliseconds/1000.0, callback);
}

void TimerMgr::disableAll(void) {
  for (int i = 0; i < _timers.size(); ++i) {
    _timers[i]->detach();
  }
}

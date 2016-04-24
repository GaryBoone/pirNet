// Copyright 2016 Gary Boone

#ifndef SRC_TIMERMGR_H_
#define SRC_TIMERMGR_H_

#include <Ticker.h>
#include <vector>

class TimerMgr {
 public:
  ~TimerMgr(void);
  void add(float seconds, Ticker::callback_t callback);
  void disableAll(void);

 private:
  std::vector<Ticker*> _timers;
};

#endif  // SRC_TIMERMGR_H_

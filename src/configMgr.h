// Copyright 2016 Gary Boone

#ifndef SRC_CONFIGMGR_H_
#define SRC_CONFIGMGR_H_

#include <FS.h>
#include "./location.h"

enum save_result_t {NO_CHANGE, SAVED, SAVE_FAILED};

class ConfigMgr {
 public:
  template <class T>
  int read(File f, T* value) const;
  template <class T>
  int write(File f, const T& value) const;
  bool readLocation(location_t* loc) const;
  bool writeLocation(location_t loc) const;
  save_result_t writeLocationIfNew(const location_t& newLoc, location_t* loc) const;
  void resetConfig(void) const;

 private:
  const char* _filename = "config.bin";
};

#endif  // SRC_CONFIGMGR_H_

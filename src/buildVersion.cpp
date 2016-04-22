// Copyright 2016 Gary Boone

#include "buildVersion.h"

// allowed: "0123456789: "
bool containsNumsColonsSpaces(const char* str) {
  if (!str) {
    return false;
  }
  while (*str) {
    if (*str != ':' && *str != ' ' && (*str - '0' < 0 || *str - '0' > 9)) {
      return false;
    }
    ++str;
  }
  return true;
}

// buildVersionString from a datetime string.
//
// Example string: Sun Sep 16 01:03:52 1973
//                 012345678901234567890123
//                           1         2
const String buildVersionString(const char* str) {
  static const char month_names[] = "???JanFebMarAprMayJunJulAugSepOctNovDec";
  if (!str || !*str || !containsNumsColonsSpaces(str + 8)) {
    return "00000000";
  }
  char mon_str[4];
  memcpy(mon_str, str + 4, 3);
  mon_str[3] = 0;
  char* month_r = strstr(month_names, mon_str);
  if (!month_r) {
    return "11111111";
  }
  int month = (strstr(month_names, month_r) - month_names) / 3;

  char* pch = const_cast<char*>(str + 8);
  const int day = static_cast<int>(strtol(pch, &pch, 10));
  const int hour = static_cast<int>(strtol(pch, &pch, 10));
  const int minute =
      static_cast<int>(strtol(pch + 1, &pch, 10));  // +1 to skip ':'
  const int second =
      static_cast<int>(strtol(pch + 1, &pch, 10));  // +1 to skip ':'
  const int year = static_cast<int>(strtol(pch, &pch, 10));

  char version[20];
  snprintf(version, sizeof(version), "%04d%02d%02d%02d%02d%02d", year, month,
           day, hour, minute, second);
  return version;
}

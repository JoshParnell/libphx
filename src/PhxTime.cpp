#include "PhxTime.h"
#include <limits.h>
#include <time.h>

inline static Time Time_Convert (struct tm* t) {
  Time result;
  result.second = t->tm_sec;
  result.minute = t->tm_min;
  result.hour = t->tm_hour;
  result.dayOfWeek = t->tm_wday + 1;
  result.dayOfMonth = t->tm_mday;
  result.dayOfYear = t->tm_yday + 1;
  result.month = t->tm_mon + 1;
  result.year = t->tm_year + 1900;
  return result;
}

Time Time_GetLocal () {
  time_t t = time(0);
  return Time_Convert(localtime(&t));
}

Time Time_GetUTC () {
  time_t t = time(0);
  return Time_Convert(gmtime(&t));
}

uint Time_GetRaw () {
  return (uint)(time(0) % UINT_MAX);
}

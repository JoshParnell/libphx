#ifndef PHX_Time
#define PHX_Time

#include "Common.h"

struct Time {
  int second;
  int minute;
  int hour;
  int dayOfWeek;
  int dayOfMonth;
  int dayOfYear;
  int month;
  int year;
};

PHX_API Time  Time_GetLocal  ();
PHX_API Time  Time_GetUTC    ();
PHX_API uint  Time_GetRaw    ();

#endif

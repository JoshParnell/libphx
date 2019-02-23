#ifndef PHX_TimeStamp
#define PHX_TimeStamp

#include "Common.h"

PHX_API TimeStamp  TimeStamp_Get            ();
PHX_API double     TimeStamp_GetDifference  (TimeStamp start, TimeStamp end);
PHX_API double     TimeStamp_GetElapsed     (TimeStamp start);
PHX_API double     TimeStamp_GetElapsedMs   (TimeStamp start);
PHX_API TimeStamp  TimeStamp_GetFuture      (double seconds);
PHX_API TimeStamp  TimeStamp_GetRelative    (TimeStamp start, double seconds);
PHX_API double     TimeStamp_ToDouble       (TimeStamp);

#endif

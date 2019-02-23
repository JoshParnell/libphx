#ifndef PHX_Profiler
#define PHX_Profiler

#include "Common.h"

#define ENABLE_PROFILER 1

PHX_API void  Profiler_Enable      ();
PHX_API void  Profiler_Disable     ();

PHX_API void  Profiler_Begin       (cstr);
PHX_API void  Profiler_End         ();
PHX_API void  Profiler_SetValue    (cstr, int);

PHX_API void  Profiler_LoopMarker  ();

PHX_API void  Profiler_Backtrace   ();

#if ENABLE_PROFILER
  #define FRAME_BEGIN Profiler_Begin(__func__)
  #define FRAME_END Profiler_End()
#else
  #define FRAME_BEGIN
  #define FRAME_END
#endif

#endif

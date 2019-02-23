#ifndef PHX_Viewport
#define PHX_Viewport

#include "Common.h"

PHX_API void   Viewport_Pop        ();
PHX_API void   Viewport_Push       (int x, int y, int sx, int sy, bool isWindow);
PHX_API float  Viewport_GetAspect  ();
PHX_API void   Viewport_GetSize    (Vec2i* out);

#endif

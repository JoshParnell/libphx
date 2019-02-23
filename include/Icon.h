#ifndef PHX_Icon
#define PHX_Icon

#include "Common.h"

PHX_API Icon* Icon_Create ();
PHX_API void  Icon_Free   (Icon*);

PHX_API void  Icon_AddBox    (Icon*, float x, float y, float sx, float sy);
PHX_API void  Icon_AddCircle (Icon*, float x, float y, float radius);
PHX_API void  Icon_AddPoint  (Icon*, float x, float y);
PHX_API void  Icon_AddLine   (Icon*, float x1, float y1, float x2, float y2);
PHX_API void  Icon_AddRing   (Icon*, float x, float y, float radius, float thickness);

PHX_API void  Icon_Draw      (Icon*, float x, float y, float size,
                              float r, float g, float b, float a);

#endif

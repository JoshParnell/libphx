#ifndef PHX_LineSegment
#define PHX_LineSegment

#include "Common.h"
#include "Vec3.h"

struct LineSegment {
  Vec3f p0;
  Vec3f p1;
};

PHX_API void  LineSegment_ToRay     (LineSegment const*, Ray*);
PHX_API void  LineSegment_FromRay   (Ray const*, LineSegment*);
PHX_API cstr  LineSegment_ToString  (LineSegment*);

#endif

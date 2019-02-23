#ifndef PHX_Triangle
#define PHX_Triangle

#include "Common.h"
#include "Vec3.h"

struct Triangle {
  Vec3f vertices[3];
};

PHX_API void   Triangle_ToPlane      (Triangle const*, Plane*);
PHX_API void   Triangle_ToPlaneFast  (Triangle const*, Plane*);
PHX_API float  Triangle_GetArea      (Triangle const*);
PHX_API Error  Triangle_Validate     (Triangle const*);

#endif

#ifndef PHX_Ray
#define PHX_Ray

#include "Common.h"
#include "Vec3.h"

struct Ray {
  Vec3f p;
  Vec3f dir;
  float tMin;
  float tMax;
};

PHX_API void  Ray_GetPoint                       (Ray const*, float t, Vec3f* out);
PHX_API bool  Ray_IntersectPlane                 (Ray const*, Plane const*, Vec3f* pHit);
PHX_API bool  Ray_IntersectTriangle_Barycentric  (Ray const*, Triangle const*, float tEpsilon, float* tHit);
PHX_API bool  Ray_IntersectTriangle_Moller1      (Ray const*, Triangle const*, float* tHit);
PHX_API bool  Ray_IntersectTriangle_Moller2      (Ray const*, Triangle const*, float* tHit);

PHX_API void  Ray_ToLineSegment                  (Ray const*, LineSegment*);
PHX_API void  Ray_FromLineSegment                (LineSegment const*, Ray*);

#endif

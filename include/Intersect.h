#ifndef PHX_Intersect
#define PHX_Intersect

#include "Common.h"
#include "LineSegment.h"

/* --- Intersect ---------------------------------------------------------------
 *
 *   Intersect_RectRect     : Rect format is {x, y, sx, sy}
 *   Intersect_RectRectFast : Only works with non-inverted rects (sx, sy > 0)
 *
 * -------------------------------------------------------------------------- */

/* NOTE : See Intersect.cpp for information on choosing these epsilons. */
const float PLANE_THICKNESS_EPSILON     = 1e-4;
const float POINT_INTERSECTION_EPSILON  = (2.0f*PLANE_THICKNESS_EPSILON);
const float RAY_INTERSECTION_EPSILON    = (8.0f*PLANE_THICKNESS_EPSILON);
const float SPHERE_INTERSECTION_EPSILON = (2.0f*PLANE_THICKNESS_EPSILON);

PHX_API bool  Intersect_PointBox                   (Matrix* t1, Matrix* t2);
PHX_API bool  Intersect_PointTriangle_Barycentric  (Vec3f const*, Triangle const*);
PHX_API bool  Intersect_RayPlane                   (Ray const*, Plane const*, Vec3f* pHit);
PHX_API bool  Intersect_RayTriangle_Barycentric    (Ray const*, Triangle const*, float tEpsilon, float* tHit);
PHX_API bool  Intersect_RayTriangle_Moller1        (Ray const*, Triangle const*, float* tHit);
PHX_API bool  Intersect_RayTriangle_Moller2        (Ray const*, Triangle const*, float* tHit);
PHX_API bool  Intersect_LineSegmentPlane           (LineSegment const*, Plane const*, Vec3f* pHit);
PHX_API bool  Intersect_RectRect                   (Vec4f const*, Vec4f const*);
PHX_API bool  Intersect_RectRectFast               (Vec4f const*, Vec4f const*);
PHX_API bool  Intersect_SphereTriangle             (Sphere const*, Triangle const*, Vec3f* pHit);

#endif

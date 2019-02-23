#include "Intersect.h"
#include "LineSegment.h"
#include "Ray.h"

void Ray_GetPoint (Ray const* self, float t, Vec3f* out) {
  *out = Vec3f_Add(self->p, Vec3f_Muls(self->dir, t));
}

bool Ray_IntersectPlane (Ray const* self, Plane const* plane, Vec3f* pHit) {
  return Intersect_RayPlane(self, plane, pHit);
}

bool Ray_IntersectTriangle_Barycentric (Ray const* self, Triangle const* tri, float tEpsilon, float* tHit) {
  return Intersect_RayTriangle_Barycentric(self, tri, tEpsilon, tHit);
}

bool Ray_IntersectTriangle_Moller1 (Ray const* self, Triangle const* tri, float* tHit) {
  return Intersect_RayTriangle_Moller1(self, tri, tHit);
}

bool Ray_IntersectTriangle_Moller2 (Ray const* self, Triangle const* tri, float* tHit) {
  return Intersect_RayTriangle_Moller2(self, tri, tHit);
}

void Ray_ToLineSegment (Ray const* self, LineSegment* lineSegment) {
  Ray_GetPoint(self, self->tMin, &lineSegment->p0);
  Ray_GetPoint(self, self->tMax, &lineSegment->p1);
}

void Ray_FromLineSegment (LineSegment const* lineSegment, Ray* self) {
  LineSegment_ToRay(lineSegment, self);
}

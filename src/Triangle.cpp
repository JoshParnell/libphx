#include "Intersect.h"
#include "Plane.h"
#include "Triangle.h"

inline static bool PointsInPlane (
  Plane const* plane,
  Triangle const* tri)
{
  for (int i = 0; i < 3; ++i) {
    float dist = Vec3f_Dot(plane->n, tri->vertices[i]) - plane->d;
    if (Abs(dist) >= PLANE_THICKNESS_EPSILON)
      return false;
  }

  return true;
};

/* TODO : OPTIMIZE */
void Triangle_ToPlane (Triangle const* tri, Plane* plane) {
  Vec3f const* v = tri->vertices;
  Vec3f e1 = Vec3f_Sub(v[1], v[0]);
  Vec3f e2 = Vec3f_Sub(v[2], v[0]);
  Vec3f n  = Vec3f_Cross(e1, e2);
  n = Vec3f_Normalize(n);

  Vec3f centroid = v[0];
  centroid = Vec3f_Add(centroid, v[1]);
  centroid = Vec3f_Add(centroid, v[2]);
  centroid = Vec3f_Divs(centroid, 3.0f);

  plane->n = n;
  plane->d = Vec3f_Dot(centroid, n);

  Assert(Plane_Validate(plane) == Error_None);
  CHECK2(Assert(PointsInPlane(plane, tri)));
}

/* NOTE: Doesn't normalize n and uses v[0] as the center. */
void Triangle_ToPlaneFast (Triangle const* triangle, Plane* plane) {
  Vec3f const* v = triangle->vertices;
  Vec3f e1 = Vec3f_Sub(v[1], v[0]);
  Vec3f e2 = Vec3f_Sub(v[2], v[0]);
  Vec3f n  = Vec3f_Cross(e1, e2);

  plane->n = n;
  plane->d = Vec3f_Dot(v[0], n);

  Assert(Plane_Validate(plane) == Error_None);
  CHECK2(Assert(PointsInPlane(plane, triangle)));
}

float Triangle_GetArea (Triangle const* tri) {
  Vec3f e1 = Vec3f_Sub(tri->vertices[1], tri->vertices[0]);
  Vec3f e2 = Vec3f_Sub(tri->vertices[2], tri->vertices[1]);
  return 0.5f * Vec3f_Length(Vec3f_Cross(e1, e2));
}

Error Triangle_Validate (Triangle const* tri) {
  Vec3f const* v = tri->vertices;

  //NaN or Inf
  for (int32 i = 0; i < 3; i++) {
    Error e = Vec3f_Validate(v[i]);
    if (e != Error_None) return Error_VertPos | e;
  }

  //Degenerate
  bool eq01 = Vec3f_Equal(v[0], v[1]);
  bool eq12 = Vec3f_Equal(v[1], v[2]);
  bool eq20 = Vec3f_Equal(v[2], v[0]);
  if (eq01 || eq12 || eq20) return Error_VertPos | Error_Degenerate;

  //Sliver
  /* TODO : The triangle should also be considered a sliver if any angle is
   *       small enough that at PLANE_THICKNESS_EPSILON from the vertex the
   *       distance between the two edges is less than PLANE_THICKNESS_EPSILON.
   *       That means 2*tan^-1(1/2) = 53.13 degrees. Yikes! Maybe each time
   *       we cut a polygon we check the newly created angles, and if any are
   *       less than this we send the triangle to both sides? */
  float e01 = Vec3f_Length(Vec3f_Sub(v[0], v[1]));
  float e12 = Vec3f_Length(Vec3f_Sub(v[1], v[2]));
  float e20 = Vec3f_Length(Vec3f_Sub(v[2], v[0]));
  float shortest = Min(Min(e01, e12), e20);
  if (shortest < 0.75f*PLANE_THICKNESS_EPSILON) return Error_VertPos | Error_Underflow;

  //Phew!
  return Error_None;
}

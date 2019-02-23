#ifndef PHX_Box3
#define PHX_Box3

#include "Common.h"
#include "Vec3.h"

struct Box3i { Vec3i lower, upper; };
struct Box3f { Vec3f lower, upper; };
struct Box3d { Vec3d lower, upper; };

#define Box3_Create(T, sub) inline T T##_Create(sub lower, sub upper) { \
  T result = { lower, upper }; return result; }

Box3_Create(Box3i, Vec3i)
Box3_Create(Box3f, Vec3f)
Box3_Create(Box3d, Vec3d)

#define Box3_Add(T, sub) inline void T##_Add(T* NO_ALIAS self, sub point) { \
  self->lower = sub##_Min(self->lower, point); \
  self->upper = sub##_Max(self->upper, point); }

Box3_Add(Box3i, Vec3i)
Box3_Add(Box3f, Vec3f)
Box3_Add(Box3d, Vec3d)

#define Box3_Center(T, sub) inline sub T##_Center(T self) { \
  sub center = { \
    (self.lower.x + self.upper.x) / 2, \
    (self.lower.y + self.upper.y) / 2, \
    (self.lower.z + self.upper.z) / 2 }; return center; }

Box3_Center(Box3f, Vec3f)
Box3_Center(Box3d, Vec3d)

#define Box3_ContainsBox(T) inline bool T##_ContainsBox(T a, T b) { \
  return a.lower.x <= b.lower.x && a.upper.x >= b.upper.x && \
         a.lower.y <= b.lower.y && a.upper.y >= b.upper.y && \
         a.lower.z <= b.lower.z && a.upper.z >= b.upper.z; }

Box3_ContainsBox(Box3i)
Box3_ContainsBox(Box3f)
Box3_ContainsBox(Box3d)

#define Box3_Extents(T, TVec) inline TVec T##_Extents(T self) { \
  return TVec##_Abs(TVec##_Sub(self.upper, self.lower)); }
Box3_Extents(Box3i, Vec3i)
Box3_Extents(Box3f, Vec3f)
Box3_Extents(Box3d, Vec3d)

#define Box3_HalfExtents(T, TVec) inline TVec T##_HalfExtents(T self) { \
  return TVec##_Muls(TVec##_Abs(TVec##_Sub(self.upper, self.lower)), 0.5f); }
//Box3_HalfExtents(Box3i, Vec3i)
Box3_HalfExtents(Box3f, Vec3f)
Box3_HalfExtents(Box3d, Vec3d)

#define Box3_Intersection(T) inline T T##_Intersection(T a, T b) { \
  T self = { \
    { Max(a.lower.x, b.lower.x), Max(a.lower.y, b.lower.y), Max(a.lower.z, b.lower.z) }, \
    { Min(a.upper.x, b.upper.x), Min(a.upper.y, b.upper.y), Min(a.upper.z, b.upper.z) }, \
  }; return self; }

Box3_Intersection(Box3i)
Box3_Intersection(Box3f)
Box3_Intersection(Box3d)

#define Box3_IntersectsBox(T) inline bool T##_IntersectsBox(T a, T b) { \
  if (a.lower.x > b.upper.x || a.upper.x < b.lower.x) return false; \
  if (a.lower.y > b.upper.y || a.upper.y < b.lower.y) return false; \
  if (a.lower.z > b.upper.z || a.upper.z < b.lower.z) return false; \
  return true; }

Box3_IntersectsBox(Box3i)
Box3_IntersectsBox(Box3f)
Box3_IntersectsBox(Box3d)

#define Box3_IntersectsRay(T, sub) inline bool T##_IntersectsRay(T self, sub ro, sub rdi) { \
  double t1 = rdi.x * (self.lower.x - ro.x); \
  double t2 = rdi.x * (self.upper.x - ro.x); \
  double tMin = Min(t1, t2); \
  double tMax = Max(t1, t2); \
  t1 = rdi.y * (self.lower.y - ro.y); \
  t2 = rdi.y * (self.upper.y - ro.y); \
  tMin = Max(tMin, Min(t1, t2)); \
  tMax = Min(tMax, Max(t1, t2)); \
  t1 = rdi.z * (self.lower.z - ro.z); \
  t2 = rdi.z * (self.upper.z - ro.z); \
  tMin = Max(tMin, Min(t1, t2)); \
  tMax = Min(tMax, Max(t1, t2)); \
  return tMax >= tMin && tMax > 0; }

Box3_IntersectsRay(Box3f, Vec3f)
Box3_IntersectsRay(Box3d, Vec3d)

#define Box3_Surface(T, sub) inline sub T##_Surface(T self) { \
  sub sx = self.upper.x - self.lower.x; \
  sub sy = self.upper.y - self.lower.y; \
  sub sz = self.upper.z - self.lower.z; \
  return 2 * (sx * sy + sx * sz + sy * sz); }

Box3_Surface(Box3i, int)
Box3_Surface(Box3f, float)
Box3_Surface(Box3d, double)

#define Box3_Union(T) inline T T##_Union(T a, T b) { \
  T self = { \
    { Min(a.lower.x, b.lower.x), Min(a.lower.y, b.lower.y), Min(a.lower.z, b.lower.z) }, \
    { Max(a.upper.x, b.upper.x), Max(a.upper.y, b.upper.y), Max(a.upper.z, b.upper.z) }, \
  }; return self; }

Box3_Union(Box3i)
Box3_Union(Box3f)
Box3_Union(Box3d)

#define Box3_Volume(T, sub) inline sub T##_Volume(T self) { \
  return (self.upper.x - self.lower.x) * \
         (self.upper.y - self.lower.y) * \
         (self.upper.z - self.lower.z); }

Box3_Volume(Box3i, int)
Box3_Volume(Box3f, float)
Box3_Volume(Box3d, double)

#endif

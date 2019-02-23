#ifndef PHX_Vec3
#define PHX_Vec3

#include "Common.h"
#include "Array.h"
#include "Error.h"
#include "PhxFloat.h"
#include "PhxMath.h"

#include <stdio.h>

/* TODO : Replace as macro-free C using codegen. */

#define UNPACK3(v) (v).x, (v).y, (v).z
#define REPEAT3(v) (v), (v), (v)

struct Vec3i { int x, y, z; };
struct Vec3d { double x, y, z; };
struct Vec3f { float x, y, z; };

#define Vec3_Create(T, sub) inline T T##_Create(sub x, sub y, sub z) { \
  T self = { x, y, z }; return self; }

#define Vec3_Rep(T, sub) inline T T##_Rep(sub r) { \
  T self = { r, r, r }; return self; }

#define Vec3_Add(T) inline T T##_Add(T a, T b) { \
  T self = { a.x + b.x, a.y + b.y, a.z + b.z }; return self; }
#define Vec3_Div(T) inline T T##_Div(T a, T b) { \
  T self = { a.x / b.x, a.y / b.y, a.z / b.z }; return self; }
#define Vec3_Mul(T) inline T T##_Mul(T a, T b) { \
  T self = { a.x * b.x, a.y * b.y, a.z * b.z }; return self; }
#define Vec3_Sub(T) inline T T##_Sub(T a, T b) { \
  T self = { a.x - b.x, a.y - b.y, a.z - b.z }; return self; }

#define Vec3_IAdd(T) inline void T##_IAdd(T* a, T b) { \
  a->x += b.x; a->y += b.y; a->z += b.z; }
#define Vec3_IDiv(T) inline void T##_IDiv(T* a, T b) { \
  a->x /= b.x; a->y /= b.y; a->z /= b.z; }
#define Vec3_IMul(T) inline void T##_IMul(T* a, T b) { \
  a->x *= b.x; a->y *= b.y; a->z *= b.z; }
#define Vec3_ISub(T) inline void T##_ISub(T* a, T b) { \
  a->x -= b.x; a->y -= b.y; a->z -= b.z; }

#define Vec3_Adds(T, sub) inline T T##_Adds(T a, sub b) { \
  T self = { a.x + b, a.y + b, a.z + b }; return self; }
#define Vec3_Divs(T, sub) inline T T##_Divs(T a, sub b) { \
  T self = { a.x / b, a.y / b, a.z / b }; return self; }
#define Vec3_Muls(T, sub) inline T T##_Muls(T a, sub b) { \
  T self = { a.x * b, a.y * b, a.z * b }; return self; }
#define Vec3_Subs(T, sub) inline T T##_Subs(T a, sub b) { \
  T self = { a.x - b, a.y - b, a.z - b }; return self; }

#define Vec3_IAdds(T, sub) inline void T##_IAdds(T* a, sub b) { \
  a->x += b; a->y += b; a->z += b; }
#define Vec3_IDivs(T, sub) inline void T##_IDivs(T* a, sub b) { \
  a->x /= b; a->y /= b; a->z /= b; }
#define Vec3_IMuls(T, sub) inline void T##_IMuls(T* a, sub b) { \
  a->x *= b; a->y *= b; a->z *= b; }
#define Vec3_ISubs(T, sub) inline void T##_ISubs(T* a, sub b) { \
  a->x -= b; a->y -= b; a->z -= b; }

Vec3_Create(Vec3i, int)
Vec3_Create(Vec3d, double)
Vec3_Create(Vec3f, float)

Vec3_Rep(Vec3i, int)
Vec3_Rep(Vec3d, double)
Vec3_Rep(Vec3f, float)

Vec3_Add(Vec3i) Vec3_Add(Vec3d) Vec3_Add(Vec3f)
Vec3_Div(Vec3i) Vec3_Div(Vec3d) Vec3_Div(Vec3f)
Vec3_Mul(Vec3i) Vec3_Mul(Vec3d) Vec3_Mul(Vec3f)
Vec3_Sub(Vec3i) Vec3_Sub(Vec3d) Vec3_Sub(Vec3f)

Vec3_IAdd(Vec3i) Vec3_IAdd(Vec3d) Vec3_IAdd(Vec3f)
Vec3_IDiv(Vec3i) Vec3_IDiv(Vec3d) Vec3_IDiv(Vec3f)
Vec3_IMul(Vec3i) Vec3_IMul(Vec3d) Vec3_IMul(Vec3f)
Vec3_ISub(Vec3i) Vec3_ISub(Vec3d) Vec3_ISub(Vec3f)

Vec3_Adds(Vec3i, int) Vec3_Adds(Vec3d, double) Vec3_Adds(Vec3f, float)
Vec3_Divs(Vec3i, int) Vec3_Divs(Vec3d, double) Vec3_Divs(Vec3f, float)
Vec3_Muls(Vec3i, int) Vec3_Muls(Vec3d, double) Vec3_Muls(Vec3f, float)
Vec3_Subs(Vec3i, int) Vec3_Subs(Vec3d, double) Vec3_Subs(Vec3f, float)

Vec3_IAdds(Vec3i, int) Vec3_IAdds(Vec3d, double) Vec3_IAdds(Vec3f, float)
Vec3_IDivs(Vec3i, int) Vec3_IDivs(Vec3d, double) Vec3_IDivs(Vec3f, float)
Vec3_IMuls(Vec3i, int) Vec3_IMuls(Vec3d, double) Vec3_IMuls(Vec3f, float)
Vec3_ISubs(Vec3i, int) Vec3_ISubs(Vec3d, double) Vec3_ISubs(Vec3f, float)

#define Vec3_Abs(T) inline T T##_Abs(T v) { \
  T self = { Abs(v.x), Abs(v.y), Abs(v.z) }; return self; }

Vec3_Abs(Vec3i)
Vec3_Abs(Vec3f)
Vec3_Abs(Vec3d)

#define Vec3_IAbs(T) inline void T##_Abs(T* v) { \
  v->x = Abs(v->x); v->y = Abs(v->y); v->z = Abs(v->z); }

Vec3_IAbs(Vec3i)
Vec3_IAbs(Vec3f)
Vec3_IAbs(Vec3d)

#define Vec3_Clamp(T) inline T T##_Clamp(T v, T lower, T upper) { \
  T self = { \
    Clamp(v.x, lower.x, upper.x), \
    Clamp(v.y, lower.y, upper.y), \
    Clamp(v.z, lower.z, upper.z), \
  }; \
  return self; }

Vec3_Clamp(Vec3i)
Vec3_Clamp(Vec3f)
Vec3_Clamp(Vec3d)

#define Vec3_Cross(T) inline T T##_Cross(T a, T b) { \
  T self = { \
    b.z * a.y - b.y * a.z, \
    b.x * a.z - b.z * a.x, \
    b.y * a.x - b.x * a.y, \
  }; \
  return self; }

Vec3_Cross(Vec3i)
Vec3_Cross(Vec3f)
Vec3_Cross(Vec3d)

#define Vec3_Equal(T) inline bool T##_Equal(T a, T b) { \
  return a.x == b.x && a.y == b.y && a.z == b.z; }

Vec3_Equal(Vec3i)
Vec3_Equal(Vec3f)
Vec3_Equal(Vec3d)

#define Vec3_Distance(T, sub) inline sub T##_Distance(T a, T b) { \
  sub dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z; \
  return Sqrt(dx * dx + dy * dy + dz * dz); }

Vec3_Distance(Vec3f, float)
Vec3_Distance(Vec3d, double)

#define Vec3_DistanceSquared(T, sub) inline sub T##_DistanceSquared(T a, T b) { \
  sub dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z; \
  return dx * dx + dy * dy + dz * dz; }

Vec3_DistanceSquared(Vec3i, int)
Vec3_DistanceSquared(Vec3f, float)
Vec3_DistanceSquared(Vec3d, double)

#define Vec3_Dot(T, sub) inline sub T##_Dot(T a, T b) { \
  return a.x * b.x + a.y * b.y + a.z * b.z; }
#define Vec3_Dots(T, sub) inline sub T##_Dots(T a, sub x, sub y, sub z) { \
  return a.x * x + a.y * y + a.z * z; }

Vec3_Dot(Vec3i, int)
Vec3_Dot(Vec3f, float)
Vec3_Dot(Vec3d, double)

Vec3_Dots(Vec3i, int)
Vec3_Dots(Vec3f, float)
Vec3_Dots(Vec3d, double)

#define Vec3_Length(T, sub) inline sub T##_Length(T v) { \
  return Sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

Vec3_Length(Vec3f, float)
Vec3_Length(Vec3d, double)

inline float Vec3i_Length(Vec3i v) {
  return Sqrt(
      (float)v.x * (float)v.x
    + (float)v.y * (float)v.y
    + (float)v.z * (float)v.z);
}

#define Vec3_LengthSquared(T, sub) inline sub T##_LengthSquared(T v) { \
  return v.x * v.x + v.y * v.y + v.z * v.z; }

Vec3_LengthSquared(Vec3i, int)
Vec3_LengthSquared(Vec3f, float)
Vec3_LengthSquared(Vec3d, double)

#define Vec3_Lerp(T, sub) inline T T##_Lerp(T a, T b, sub t) { \
  T self = { \
    a.x + (b.x - a.x) * t, \
    a.y + (b.y - a.y) * t, \
    a.z + (b.z - a.z) * t, \
  }; \
  return self; }

Vec3_Lerp(Vec3f, float)
Vec3_Lerp(Vec3d, double)

#define Vec3_Max(T) inline T T##_Max(T a, T b) { \
  T self = { Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z) }; return self; }

#define Vec3_Min(T) inline T T##_Min(T a, T b) { \
  T self = { Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z) }; return self; }

Vec3_Max(Vec3i)
Vec3_Max(Vec3f)
Vec3_Max(Vec3d)
Vec3_Min(Vec3i)
Vec3_Min(Vec3f)
Vec3_Min(Vec3d)

#define Vec3_Normalize(T, sub) inline T T##_Normalize(T v) { \
  sub l = T##_Length(v); \
  T self = { v.x / l, v.y / l, v.z / l }; return self; }

#define Vec3_SNormalize(T, sub) inline T T##_SNormalize(T v) { \
  sub l = T##_Length(v); \
  if (l > 0) { T self = { v.x / l, v.y / l, v.z / l }; return self; } \
  return v; }

Vec3_Normalize(Vec3f, float)
Vec3_Normalize(Vec3d, double)
Vec3_SNormalize(Vec3f, float)
Vec3_SNormalize(Vec3d, double)

#define Vec3_Pow(T) inline T T##_Pow(T a, T b) { \
  T self = { Pow(a.x, b.x), Pow(a.y, b.y), Pow(a.z, b.z) }; return self; }
#define Vec3_Pows(T, sub) inline T T##_Pows(T a, sub b) { \
  T self = { Pow(a.x, b), Pow(a.y, b), Pow(a.z, b) }; return self; }

Vec3_Pow(Vec3d)
Vec3_Pow(Vec3f)
Vec3_Pows(Vec3d, double)
Vec3_Pows(Vec3f, float)

#define Vec3_Project(T, sub) inline T T##_Project(T a, T b) { \
  sub d = T##_Dot(a, b); \
  T self = { d * b.x, d * b.y, d * b.z }; return self; }

Vec3_Project(Vec3i, int)
Vec3_Project(Vec3f, float)
Vec3_Project(Vec3d, double)

#define Vec3_Rcp(T, sub) inline T T##_Rcp(T a) { \
  T self = { \
    (sub)(1.0 / a.x), \
    (sub)(1.0 / a.y), \
    (sub)(1.0 / a.z), \
  }; \
  return self; }

Vec3_Rcp(Vec3i, int)
Vec3_Rcp(Vec3d, double)
Vec3_Rcp(Vec3f, float)

#define Vec3_Reject(T, sub) inline T T##_Reject(T a, T b) { \
  sub d = T##_Dot(a, b); \
  T self = { a.x - d * b.x, a.y - d * b.y, a.z - d * b.z }; return self; }

Vec3_Reject(Vec3i, int)
Vec3_Reject(Vec3f, float)
Vec3_Reject(Vec3d, double)

#define Vec3_To(T, TSub, CSub) inline TSub T##_To##TSub(T a) { \
  TSub self = { (CSub) a.x, (CSub) a.y, (CSub) a.z, }; return self; }

Vec3_To(Vec3i, Vec3f, float)
Vec3_To(Vec3i, Vec3d, double)
Vec3_To(Vec3f, Vec3i, int)
Vec3_To(Vec3f, Vec3d, double)
Vec3_To(Vec3d, Vec3i, int)
Vec3_To(Vec3d, Vec3f, float)

#define Vec3_Validate(T, sub) inline Error T##_Validate(T v) { \
  Error e = Error_None; \
  e |= Float_Validate(v.x); \
  e |= Float_Validate(v.y); \
  e |= Float_Validate(v.z); \
  return e; }

Vec3_Validate(Vec3f, float)
Vec3_Validate(Vec3d, double)

#define Vec3_ToString(T, sub, fmt) inline cstr T##_ToString(T* v) { \
  static char buffer[512]; \
  snprintf(buffer, (size_t) Array_GetSize(buffer), \
    "(" fmt ", " fmt ", " fmt ")", v->x, v->y, v->z); \
  return buffer; }

Vec3_ToString(Vec3i, int,    "%i")
Vec3_ToString(Vec3f, float,  "%.4f")
Vec3_ToString(Vec3d, double, "%.4f")

#endif

#ifndef PHX_Vec4
#define PHX_Vec4

#include "Common.h"
#include "Error.h"
#include "PhxFloat.h"
#include "Vec2.h"
#include "Vec3.h"

/* TODO : Replace as macro-free C using codegen. */

#define UNPACK4(v) (v).x, (v).y, (v).z, (v).w
#define REPEAT4(v) (v), (v), (v), (v)

struct Vec4i { int x, y, z, w; };
struct Vec4f { float x, y, z, w; };
struct Vec4d { double x, y, z, w; };

#define Vec4_Create(T, sub) inline T T##_Create(sub x, sub y, sub z, sub w) { \
  T self = { x, y, z, w }; return self; }

Vec4_Create(Vec4i, int)
Vec4_Create(Vec4d, double)
Vec4_Create(Vec4f, float)

#define Vec4_Abs(T) inline T T##_Abs(T v) { \
  T self = { Abs(v.x), Abs(v.y), Abs(v.z), Abs(v.w) }; return self; }

Vec4_Abs(Vec4i)
Vec4_Abs(Vec4f)
Vec4_Abs(Vec4d)

#define Vec4_IAbs(T) inline void T##_Abs(T* v) { \
  v->x = Abs(v->x); v->y = Abs(v->y); v->z = Abs(v->z); v->w = Abs(v->w); }

Vec4_IAbs(Vec4i)
Vec4_IAbs(Vec4f)
Vec4_IAbs(Vec4d)

#define Vec4_Equal(T) inline bool T##_Equal(T a, T b) { \
  return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

Vec4_Equal(Vec4i) Vec4_Equal(Vec4f) Vec4_Equal(Vec4d)

#define Vec4_Sub(T) inline T T##_Sub(T a, T b) { \
  T self = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; return self; }

Vec4_Sub(Vec4i) Vec4_Sub(Vec4d) Vec4_Sub(Vec4f)

#define Vec4_To3(T, TSub, CSub) inline TSub T##_To##TSub(T a) { \
  TSub self = { (CSub) a.x, (CSub) a.y, (CSub) a.z, }; return self; }

Vec4_To3(Vec4i, Vec3f, float)
Vec4_To3(Vec4i, Vec3d, double)
Vec4_To3(Vec4d, Vec3i, int)
Vec4_To3(Vec4d, Vec3f, float)
Vec4_To3(Vec4f, Vec3i, int)
Vec4_To3(Vec4f, Vec3d, double)

#define Vec4_XYZ(T, U) inline U T##_XYZ(T a) { \
  U self = { a.x, a.y, a.z }; return self; }

Vec4_XYZ(Vec4i, Vec3i)
Vec4_XYZ(Vec4d, Vec3d)
Vec4_XYZ(Vec4f, Vec3f)

#define Vec4_XY(T, U) inline U T##_XY(T a) { U self = { a.x, a.y }; return self; }
#define Vec4_ZW(T, U) inline U T##_ZW(T a) { U self = { a.z, a.w }; return self; }

Vec4_XY(Vec4i, Vec2i)
Vec4_ZW(Vec4i, Vec2i)
Vec4_XY(Vec4d, Vec2d)
Vec4_ZW(Vec4d, Vec2d)
Vec4_XY(Vec4f, Vec2f)
Vec4_ZW(Vec4f, Vec2f)

#define Vec4_Validate(T, sub) inline Error T##_Validate(T v) { \
  Error e = Error_None; \
  e |= Float_Validate(v.x); \
  e |= Float_Validate(v.y); \
  e |= Float_Validate(v.z); \
  e |= Float_Validate(v.w); \
  return e; }

Vec4_Validate(Vec4f, float)
Vec4_Validate(Vec4d, double)

#define Vec4_ToString(T, sub, fmt) inline cstr T##_ToString(T* v) { \
  static char buffer[512]; \
  snprintf(buffer, (size_t) Array_GetSize(buffer), \
    "(" fmt ", " fmt ", " fmt ", " fmt ")", v->x, v->y, v->z, v->w); \
  return buffer; }

Vec4_ToString(Vec4i, int,    "%i")
Vec4_ToString(Vec4f, float,  "%.4f")
Vec4_ToString(Vec4d, double, "%.4f")

#endif

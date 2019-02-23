#ifndef PHX_Vec2
#define PHX_Vec2

#include "Common.h"
#include "Array.h"
#include "Error.h"
#include "PhxMath.h"
#include "PhxFloat.h"

#include <stdio.h>

/* TODO : Replace as macro-free C using codegen. */

#define UNPACK2(v) (v).x, (v).y
#define REPEAT2(v) (v), (v)

struct Vec2i { int x, y; };
struct Vec2d { double x, y; };
struct Vec2f { float x, y; };

#define Vec2_Create(T, sub) inline T T##_Create(sub x, sub y) { \
  T self = { x, y }; return self; }

#define Vec2_Add(T) inline T T##_Add(T a, T b) { \
  T self = { a.x + b.x, a.y + b.y }; return self; }
#define Vec2_Div(T) inline T T##_Div(T a, T b) { \
  T self = { a.x / b.x, a.y / b.y }; return self; }
#define Vec2_Mul(T) inline T T##_Mul(T a, T b) { \
  T self = { a.x * b.x, a.y * b.y }; return self; }
#define Vec2_Sub(T) inline T T##_Sub(T a, T b) { \
  T self = { a.x - b.x, a.y - b.y }; return self; }

#define Vec2_IAdd(T) inline void T##_IAdd(T* a, T b) { \
  a->x += b.x; a->y += b.y; }
#define Vec2_IDiv(T) inline void T##_IDiv(T* a, T b) { \
  a->x /= b.x; a->y /= b.y; }
#define Vec2_IMul(T) inline void T##_IMul(T* a, T b) { \
  a->x *= b.x; a->y *= b.y; }
#define Vec2_ISub(T) inline void T##_ISub(T* a, T b) { \
  a->x -= b.x; a->y -= b.y; }

#define Vec2_Adds(T, sub) inline T T##_Adds(T a, sub b) { \
  T self = { a.x + b, a.y + b }; return self; }
#define Vec2_Divs(T, sub) inline T T##_Divs(T a, sub b) { \
  T self = { a.x / b, a.y / b }; return self; }
#define Vec2_Muls(T, sub) inline T T##_Muls(T a, sub b) { \
  T self = { a.x * b, a.y * b }; return self; }
#define Vec2_Subs(T, sub) inline T T##_Subs(T a, sub b) { \
  T self = { a.x - b, a.y - b }; return self; }

#define Vec2_IAdds(T, sub) inline void T##_IAdds(T* a, sub b) { \
  a->x += b; a->y += b; }
#define Vec2_IDivs(T, sub) inline void T##_IDivs(T* a, sub b) { \
  a->x /= b; a->y /= b; }
#define Vec2_IMuls(T, sub) inline void T##_IMuls(T* a, sub b) { \
  a->x *= b; a->y *= b; }
#define Vec2_ISubs(T, sub) inline void T##_ISubs(T* a, sub b) { \
  a->x -= b; a->y -= b; }

Vec2_Create(Vec2i, int)
Vec2_Create(Vec2d, double)
Vec2_Create(Vec2f, float)

Vec2_Add(Vec2i) Vec2_Add(Vec2d) Vec2_Add(Vec2f)
Vec2_Div(Vec2i) Vec2_Div(Vec2d) Vec2_Div(Vec2f)
Vec2_Mul(Vec2i) Vec2_Mul(Vec2d) Vec2_Mul(Vec2f)
Vec2_Sub(Vec2i) Vec2_Sub(Vec2d) Vec2_Sub(Vec2f)

Vec2_IAdd(Vec2i) Vec2_IAdd(Vec2d) Vec2_IAdd(Vec2f)
Vec2_IDiv(Vec2i) Vec2_IDiv(Vec2d) Vec2_IDiv(Vec2f)
Vec2_IMul(Vec2i) Vec2_IMul(Vec2d) Vec2_IMul(Vec2f)
Vec2_ISub(Vec2i) Vec2_ISub(Vec2d) Vec2_ISub(Vec2f)

Vec2_Adds(Vec2i, int) Vec2_Adds(Vec2d, double) Vec2_Adds(Vec2f, float)
Vec2_Divs(Vec2i, int) Vec2_Divs(Vec2d, double) Vec2_Divs(Vec2f, float)
Vec2_Muls(Vec2i, int) Vec2_Muls(Vec2d, double) Vec2_Muls(Vec2f, float)
Vec2_Subs(Vec2i, int) Vec2_Subs(Vec2d, double) Vec2_Subs(Vec2f, float)

Vec2_IAdds(Vec2i, int) Vec2_IAdds(Vec2d, double) Vec2_IAdds(Vec2f, float)
Vec2_IDivs(Vec2i, int) Vec2_IDivs(Vec2d, double) Vec2_IDivs(Vec2f, float)
Vec2_IMuls(Vec2i, int) Vec2_IMuls(Vec2d, double) Vec2_IMuls(Vec2f, float)
Vec2_ISubs(Vec2i, int) Vec2_ISubs(Vec2d, double) Vec2_ISubs(Vec2f, float)

#define Vec2_Abs(T) inline T T##_Abs(T v) { \
  T self = { Abs(v.x), Abs(v.y) }; return self; }

Vec2_Abs(Vec2i)
Vec2_Abs(Vec2f)
Vec2_Abs(Vec2d)

#define Vec2_IAbs(T) inline void T##_Abs(T* v) { \
  v->x = Abs(v->x); v->y = Abs(v->y); }

Vec2_IAbs(Vec2i)
Vec2_IAbs(Vec2f)
Vec2_IAbs(Vec2d)

#define Vec2_Equal(T) inline bool T##_Equal(T a, T b) { \
  bool self = a.x == b.x && a.y == b.y; \
  return self; }

Vec2_Equal(Vec2i)
Vec2_Equal(Vec2f)
Vec2_Equal(Vec2d)

#define Vec2_Length(T, sub) inline sub T##_Length(T v) { \
  return Sqrt(v.x * v.x + v.y * v.y); }

Vec2_Length(Vec2f, float)
Vec2_Length(Vec2d, double)

#define Vec2_LengthSquared(T, sub) inline sub T##_LengthSquared(T v) { \
  return v.x * v.x + v.y * v.y; }

Vec2_LengthSquared(Vec2f, float)
Vec2_LengthSquared(Vec2d, double)

#define Vec2_Normalize(T, sub) inline T T##_Normalize(T v) { \
  sub l = T##_Length(v); \
  T self = { v.x / l, v.y / l }; return self; }

Vec2_Normalize(Vec2f, float)
Vec2_Normalize(Vec2d, double)

#define Vec2_Dot(T, sub) inline sub T##_Dot(T a, T b) { \
  return a.x * b.x + a.y * b.y; }

Vec2_Dot(Vec2i, int)
Vec2_Dot(Vec2f, float)
Vec2_Dot(Vec2d, double)

#define Vec2_Validate(T, sub) inline Error T##_Validate(T v) { \
  Error e = Error_None; \
  e |= Float_Validate(v.x); \
  e |= Float_Validate(v.y); \
  return e; }

Vec2_Validate(Vec2f, float)
Vec2_Validate(Vec2d, double)

#define Vec2_ToString(T, sub, fmt) inline cstr T##_ToString(T* v) { \
  static char buffer[512]; \
  snprintf(buffer, (size_t) Array_GetSize(buffer), \
    "(" fmt ", " fmt ")", v->x, v->y); \
  return buffer; }

Vec2_ToString(Vec2i, int,    "%i")
Vec2_ToString(Vec2f, float,  "%.4f")
Vec2_ToString(Vec2d, double, "%.4f")

#endif

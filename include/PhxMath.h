#ifndef PHX_Math
#define PHX_Math

#include "Common.h"

#include <math.h>
#include <stdlib.h>

/* -- Float/Double ---------------------------------------------------------- */

const float  Tau         = 6.28318531f;
const float  Pi          = 3.14159265f;
const float  Pi2         = 1.57079633f;
const float  Pi4         = 0.785398163f;
const float  Pi6         = 0.523598776f;
const float  F32_EPSILON = 1.19209290e-07f;
const double F64_EPSILON = 2.2204460492503131e-16;

inline bool   Approx     (float a, float b);
inline float  Acos       (float t);
inline float  Asin       (float t);
inline float  Atan       (float t);
inline float  Atan       (float y, float x);
inline float  Ceil       (float t);
inline float  Cos        (float t);
inline float  Degrees    (float radians);
inline float  Exp        (float t);
inline float  Floor      (float t);
inline float  Fract      (float t);
inline float  Lerp       (float a, float b, float t);
inline float  Log        (float t);
inline float  Log        (float t, float b);
inline bool   NonZero    (float t);
inline float  Pow        (float t, float p);
inline float  Pow2       (float t);
inline float  Pow4       (float t);
inline float  Pow8       (float t);
inline float  Radians    (float degrees);
inline float  Round      (float t);
inline float  Round      (float t, int places);
inline float  Saturate   (float t);
inline float  SignPow    (float t, float p);
inline float  Sin        (float t);
inline float  Sqrt       (float t);
inline float  Step       (float t, float edge);
inline float  Tan        (float t);

/* -- Int/Float/Double  ----------------------------------------------------- */

inline int    Abs        (int t);
inline int    Clamp      (int t, int lower, int upper);
inline int    Clamp01    (int t);
inline int    ClampUnit  (int t);
inline int    Max        (int a, int b);
inline int    Min        (int a, int b);
inline int    Mod        (int i, int m);
inline int    Sign       (int t);

/*
 *   Functions suffixed with 'Signed' are signed:
 *     FnSigned (x, ...)              Sign(x) * Fn(Abs(x), ...)
 *
 *   Functions marked with ! are specialized variants of another function.
 *   Specializations are provided only when doing so affords a significant
 *   performance benefit. Thus, functions marked with ! should be preferred
 *   over their generic variants when applicable:
 *
 *       Fn
 *     ! FnBlah  <-- Strictly more efficient than Fn
 *
 *     ExpMap (x, p)                  1.0 - Exp(-Pow(Abs(x), p))
 *   ! ExpMap1 (x)                    ExpMap(x, 1)
 *   ! ExpMap2 (x)                    ExpMap(x, 2)
 *
 *     PowSigned (x, p)               Sign(x) * Pow(Abs(x), p)
 *
 *     Sign(x)                        |  1  x > 0
 *                                    |  0  x = 0
 *                                    | -1  x < 0
 *
 */

PHX_API double  Math_Bezier3        (double x, double, double, double);
PHX_API double  Math_Bezier4        (double x, double, double, double, double);
PHX_API double  Math_Clamp          (double x, double a, double b);
PHX_API double  Math_Clamp01        (double x);
PHX_API double  Math_ClampSafe      (double x, double a, double b);
PHX_API double  Math_ClampUnit      (double x);
PHX_API double  Math_ExpMap         (double x, double p);
PHX_API double  Math_ExpMapSigned   (double x, double p);
PHX_API double  Math_ExpMap1        (double x);
PHX_API double  Math_ExpMap1Signed  (double x);
PHX_API double  Math_ExpMap2        (double x);
PHX_API double  Math_ExpMap2Signed  (double x);
PHX_API double  Math_PowSigned      (double x, double p);
PHX_API double  Math_Round          (double x);
PHX_API double  Math_Sign           (double x);

/* -------------------------------------------------------------------------- */

/* Units. */
inline float Degrees (float radians) {
  return (180.0f / Pi) * radians;
}

inline double Degrees (double radians) {
  return (180.0 / Pi) * radians;
}

inline float Radians (float degrees) {
  return (Pi / 180.0f) * degrees;
}

inline double Radians (double degrees) {
  return (Pi / 180.0) * degrees;
}

/* General. */
inline int Abs (int t) {
  return t < 0 ? -t : t;
}

inline float Abs (float t) {
  return (float)fabs((double)t);
}

inline double Abs (double t) {
  return fabs(t);
}

inline bool Approx (float a, float b) {
  return fabs(a - b) < 1e-4;
}

inline bool Approx (double a, double b) {
  return fabs(a - b) < 1e-4;
}

inline float Ceil (float t) {
  return (float)ceil((double)t);
}

inline double Ceil (double t) {
  return ceil(t);
}

inline int Clamp (int t, int lower, int upper) {
  t = t > upper ? upper : t;
  t = t < lower ? lower : t;
  return t;
}

inline float Clamp (float t, float lower, float upper) {
  t = t > upper ? upper : t;
  t = t < lower ? lower : t;
  return t;
}

inline double Clamp (double t, double lower, double upper) {
  t = t > upper ? upper : t;
  t = t < lower ? lower : t;
  return t;
}

inline int Clamp01 (int t) {
  t = t > 1 ? 1 : t;
  t = t < 0 ? 0 : t;
  return t;
}

inline float Clamp01 (float t) {
  t = t > 1.0f ? 1.0f : t;
  t = t < 0.0f ? 0.0f : t;
  return t;
}

inline double Clamp01 (double t) {
  t = t > 1.0 ? 1.0 : t;
  t = t < 0.0 ? 0.0 : t;
  return t;
}

inline int ClampUnit (int t) {
  t = t >  1 ?  1 : t;
  t = t < -1 ? -1 : t;
  return t;
}

inline float ClampUnit (float t) {
  t = t >  1.0f ?  1.0f : t;
  t = t < -1.0f ? -1.0f : t;
  return t;
}

inline double ClampUnit (double t) {
  t = t >  1.0 ?  1.0 : t;
  t = t < -1.0 ? -1.0 : t;
  return t;
}

inline float Exp (float t) {
  return (float)exp((double)t);
}

inline double Exp (double t) {
  return exp(t);
}

inline float Floor (float t) {
  return (float)floor((double)t);
}

inline double Floor (double t) {
  return floor(t);
}

inline float Fract (float t) {
  return t - Floor(t);
}

inline double Fract (double t) {
  return t - Floor(t);
}

inline float Lerp (float a, float b, float t) {
  return a + t * (b - a);
}

inline double Lerp (double a, double b, double t) {
  return a + t * (b - a);
}

inline float Log (float t) {
  return (float)log((double)t);
}

inline double Log (double t) {
  return log(t);
}

inline float Log (float t, float b) {
  return Log(t) / Log(b);
}

inline double Log (double t, double b) {
  return Log(t) / Log(b);
}

inline uint32 Max (uint32 a, uint32 b) {
  return a > b ? a : b;
}

inline int Max (int a, int b) {
  return a > b ? a : b;
}

inline float Max (float a, float b) {
  return a > b ? a : b;
}

inline double Max (double a, double b) {
  return a > b ? a : b;
}

inline int Min (int a, int b) {
  return a < b ? a : b;
}

inline float Min (float a, float b) {
  return a < b ? a : b;
}

inline double Min (double a, double b) {
  return a < b ? a : b;
}

inline int Mod (int i, int m) {
  return i % m;
}

inline float Mod (float t, float m) {
  return (float)fmod((double)t, (double)m);
}

inline double Mod (double t, double m) {
  return fmod(t, m);
}

inline bool NonZero (float t) {
  return Abs(t) > F32_EPSILON;
}

inline bool NonZero (double t) {
  return Abs(t) > F64_EPSILON;
}

inline float Pow (float t, float p) {
  return (float)pow((double)t, (double)p);
}

inline double Pow (double t, double p) {
  return pow(t, p);
}

inline float Pow2 (float t) {
  return t * t;
}

inline double Pow2 (double t) {
  return t * t;
}

inline float Pow4 (float t) {
  float t2 = t * t;
  return t2 * t2;
}

inline double Pow4 (double t) {
  double t2 = t * t;
  return t2 * t2;
}

inline float Pow8 (float t) {
  float t2 = t * t;
  float t4 = t2 * t2;
  return t4 * t4;
}

inline double Pow8 (double t) {
  double t2 = t * t;
  double t4 = t2 * t2;
  return t4 * t4;
}

inline float Round (float t) {
  return Floor(t + 0.5f);
}

inline double Round (double t) {
  return Floor(t + 0.5);
}

inline float Round (float t, int places) {
  double factor = 10.0;
  for (int i = 0; i < places; ++i)
    factor *= 10.0;
  return (float)(Floor(factor * t + 0.5) / factor);
}

inline double Round (double t, int places) {
  double factor = 10.0;
  for (int i = 0; i < places; ++i)
    factor *= 10.0;
  return Floor(factor * t + 0.5) / factor;
}

inline float Saturate (float t) {
  return t < 0.0f ? 0.0f :
         t > 1.0f ? 1.0f :
         t;
}

inline double Saturate (double t) {
  return t < 0.0 ? 0.0 :
         t > 1.0 ? 1.0 :
         t;
}

inline int Sign (int t) {
  return t  > 0 ? 1 :
         t == 0 ? 0 : -1;
}

inline float Sign (float t) {
  return t  > 0.0f ? 1.0f :
         t == 0.0f ? 0.0f : -1.0f;
}

inline double Sign (double t) {
  return t  > 0.0 ? 1.0 :
         t == 0.0 ? 0.0 : -1.0;
}

inline float SignPow (float t, float p) {
  return Sign(t) * Pow(Abs(t), p);
}

inline double SignPow (double t, double p) {
  return Sign(t) * Pow(Abs(t), p);
}

inline float Step (float t, float edge) {
  return t < edge ? 0.0f : 1.0f;
}

inline double Step (double t, double edge) {
  return t < edge ? 0.0 : 1.0;
}

inline float Sqrt (float t) {
  return (float)sqrt((double)t);
}

inline double Sqrt (double t) {
  return sqrt(t);
}

/* Trig. */
inline float Acos (float t) {
  return (float)acos((double)t);
}

inline double Acos (double t) {
  return acos(t);
}

inline float Asin (float t) {
  return (float)asin((double)t);
}

inline double Asin (double t) {
  return asin(t);
}

inline float Atan (float t) {
  return (float)atan((double)t);
}

inline double Atan (double t) {
  return atan(t);
}

inline float Atan (float y, float x) {
  return (float)atan2((double)y, (double)x);
}

inline double Atan (double y, double x) {
  return atan2(y, x);
}

inline float Cos (float t) {
  return (float)cos((double)t);
}

inline double Cos (double t) {
  return cos(t);
}

inline float Sin (float t) {
  return (float)sin((double)t);
}

inline double Sin (double t) {
  return sin(t);
}

inline float Tan (float t) {
  return (float)tan((double)t);
}

inline double Tan (double t) {
  return tan(t);
}

#endif

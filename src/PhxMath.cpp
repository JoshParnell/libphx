#include "Common.h"
#include "PhxMath.h"

/* 2-valued sign for when product with another term will provide the zero. */
inline static double Sign2 (double x) {
  return x > 0.0 ? 1.0 : -1.0;
}

double Math_Bezier3 (double x, double y1, double y2, double y3) {
  double y12 = Lerp(y1, y2, x);
  double y23 = Lerp(y2, y3, x);
  return Lerp(y12, y23, x);
}

double Math_Bezier4 (double x, double y1, double y2, double y3, double y4) {
  double y12 = Lerp(y1, y2, x);
  double y23 = Lerp(y2, y3, x);
  double y34 = Lerp(y3, y4, x);
  double y123 = Lerp(y12, y23, x);
  double y234 = Lerp(y23, y34, x);
  return Lerp(y123, y234, x);
}

double Math_Clamp (double x, double a, double b) {
  // NOTE : Not stable when b < a. Use Math_ClampSafe
  return x < a ? a : x > b ? b : x;
}

double Math_Clamp01 (double x) {
  return x < 0.0 ? 0.0 : x > 1.0 ? 1.0 : x;
}

double Math_ClampSafe (double x, double a, double b) {
  if (b < a) Swap(a, b);
  return x < a ? a : x > b ? b : x;
}

double Math_ClampUnit (double x) {
  return x < -1.0 ? -1.0 : x > 1.0 ? 1.0 : x;
}

double Math_ExpMap (double x, double p) {
  return 1.0 - Exp(-Pow(Abs(x), p));
}

double Math_ExpMapSigned (double x, double p) {
  return Sign2(x) * (1.0 - Exp(-Pow(Abs(x), p)));
}

double Math_ExpMap1 (double x) {
  return 1.0 - Exp(-Abs(x));
}

double Math_ExpMap1Signed (double x) {
  return Sign2(x) * (1.0 - Exp(-Abs(x)));
}

double Math_ExpMap2 (double x) {
  return 1.0 - Exp(-x * x);
}

double Math_ExpMap2Signed (double x) {
  return Sign2(x) * (1.0 - Exp(-x * x));
}

double Math_PowSigned (double x, double p) {
  return Sign2(x) * Pow(Abs(x), p);
}

double Math_Round (double x) {
  return Round(x);
}

double Math_Sign (double x) {
  return x > 0.0 ? 1.0 : x < 0.0 ? -1.0 : 0.0;
}

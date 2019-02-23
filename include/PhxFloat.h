#ifndef PHX_Float
#define PHX_Float

#include "Common.h"
#include "Error.h"
#include "PhxMath.h"

#include <float.h>
#include <math.h>

inline Error  Float_Validate            (float x);
inline Error  Float_Validate            (double x);
inline bool   Float_ApproximatelyEqual  (float x, float y);
inline bool   Float_ApproximatelyEqual  (double x, double y);

/* -------------------------------------------------------------------------- */

inline Error Float_Validate (float x) {
  int classification = fpclassify(x);
  switch (classification) {
    case FP_INFINITE:  return Error_Overflow;
    /* NOTE : Denormals are allowed until they become a perf issue. */
    //case FP_SUBNORMAL: return Error_Underflow;
    case FP_SUBNORMAL: break;
    case FP_NAN:       return Error_NaN;

    case FP_ZERO:
    case FP_NORMAL:   return Error_None;

    default:
      Fatal("Float_Validate: Unhandled case: %i", classification);
  }
  return Error_None;
}

inline Error Float_Validate (double x) {
  int classification = fpclassify(x);
  switch (classification) {
    case FP_INFINITE:  return Error_Overflow;
    case FP_SUBNORMAL: return Error_Underflow;
    case FP_NAN:       return Error_NaN;

    case FP_ZERO:
    case FP_NORMAL:    return Error_None;

    default:
      Fatal("Float_Validate: Unhandled case: %i", classification);
  }
  return Error_None;
}

inline bool Float_ApproximatelyEqual (float x, float y) {
  return Abs(x - y) < 1e-3f;
}

inline bool Float_ApproximatelyEqual (double x, double y) {
  return Abs(x - y) < 1e-3;
}


#endif

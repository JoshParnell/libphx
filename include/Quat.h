#ifndef PHX_Quat
#define PHX_Quat

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *   Unlike matrices, Quats are PRE-multiplicative with respect to vectors.
 *   Hence, Quat_Mul(A, B) procudes a quaternion that rotates first by A, THEN
 *   by B.
 *
 * -------------------------------------------------------------------------- */

struct Quat {
  float x;
  float y;
  float z;
  float w;
};

PHX_API Quat   Quat_Create              (float x, float y, float z, float w);

PHX_API void   Quat_GetAxisX            (Quat const*, Vec3f*);
PHX_API void   Quat_GetAxisY            (Quat const*, Vec3f*);
PHX_API void   Quat_GetAxisZ            (Quat const*, Vec3f*);
PHX_API void   Quat_GetForward          (Quat const*, Vec3f*);
PHX_API void   Quat_GetRight            (Quat const*, Vec3f*);
PHX_API void   Quat_GetUp               (Quat const*, Vec3f*);

PHX_API void   Quat_Identity            (Quat*);

PHX_API void   Quat_Canonicalize        (Quat const*, Quat* out);
PHX_API void   Quat_ICanonicalize       (Quat*);
PHX_API float  Quat_Dot                 (Quat const*, Quat const*);
PHX_API bool   Quat_Equal               (Quat const*, Quat const*);
PHX_API bool   Quat_ApproximatelyEqual  (Quat const*, Quat const*);
PHX_API void   Quat_Inverse             (Quat const*, Quat* out);
PHX_API void   Quat_IInverse            (Quat*);
PHX_API void   Quat_Lerp                (Quat const*, Quat const*, float t, Quat* out);
PHX_API void   Quat_ILerp               (Quat*, Quat const*, float t);
PHX_API void   Quat_Mul                 (Quat const*, Quat const*, Quat* out);
PHX_API void   Quat_IMul                (Quat*, Quat const*);
PHX_API void   Quat_MulV                (Quat const*, Vec3f const*, Vec3f* out);
PHX_API void   Quat_Normalize           (Quat const*, Quat* out);
PHX_API void   Quat_INormalize          (Quat*);
PHX_API void   Quat_Scale               (Quat const*, float, Quat* out);
PHX_API void   Quat_IScale              (Quat*, float);
PHX_API void   Quat_Slerp               (Quat const*, Quat const*, float, Quat* out);
PHX_API void   Quat_ISlerp              (Quat*, Quat const*, float);

PHX_API cstr   Quat_ToString            (Quat const*);
PHX_API Error  Quat_Validate            (Quat const*);

PHX_API void   Quat_FromAxisAngle       (Vec3f const* axis, float radians, Quat*);
PHX_API void   Quat_FromBasis           (Vec3f const* x, Vec3f const* y, Vec3f const* z, Quat*);
PHX_API void   Quat_FromLookUp          (Vec3f const* look, Vec3f const* up, Quat*);
//PHX_API void   Quat_FromMatrix          ();
//PHX_API void   Quat_ToMatrix            ();
PHX_API void   Quat_FromRotateTo        (Vec3f const* from, Vec3f const* to, Quat*);

#endif

/* TODO : Quat_FromAxisAngle takes radians. Ensure this matches the rest of the engine API */

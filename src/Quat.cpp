#include "PhxMath.h"
#include "PhxFloat.h"
#include "Quat.h"
#include "Vec3.h"

#include <stdio.h>

Quat Quat_Create (float x, float y, float z, float w) {
  Quat self;
  self.x = x;
  self.y = y;
  self.z = z;
  self.w = w;
  return self;
}

void Quat_GetAxisX (Quat const* q, Vec3f* out) {
  out->x = 1.0f - 2.0f * (q->y*q->y + q->z*q->z);
  out->y =        2.0f * (q->x*q->y + q->z*q->w);
  out->z =        2.0f * (q->x*q->z - q->y*q->w);
  Assert(Vec3f_Validate(*out) == Error_None);
}

void Quat_GetAxisY (Quat const* q, Vec3f* out) {
  out->x =        2.0f * (q->x*q->y - q->z*q->w);
  out->y = 1.0f - 2.0f * (q->x*q->x + q->z*q->z);
  out->z =        2.0f * (q->y*q->z + q->x*q->w);
  Assert(Vec3f_Validate(*out) == Error_None);
}

void Quat_GetAxisZ (Quat const* q, Vec3f* out) {
  out->x =        2.0f * (q->x*q->z + q->y*q->w);
  out->y =        2.0f * (q->y*q->z - q->x*q->w);
  out->z = 1.0f - 2.0f * (q->x*q->x + q->y*q->y);
  Assert(Vec3f_Validate(*out) == Error_None);
}

void Quat_GetForward (Quat const* q, Vec3f* out) {
  Quat_GetAxisZ(q, out);
  out->x = -out->x;
  out->y = -out->y;
  out->z = -out->z;
}

void Quat_GetRight (Quat const* q, Vec3f* out) {
  Quat_GetAxisX(q, out);
}

void Quat_GetUp (Quat const* q, Vec3f* out) {
  Quat_GetAxisY(q, out);
}

void Quat_Identity (Quat* out) {
  out->x = 0.0f;
  out->y = 0.0f;
  out->z = 0.0f;
  out->w = 1.0f;
}

void Quat_Canonicalize (Quat const* q, Quat* out) {
  float value = !Float_ApproximatelyEqual(q->w, 0.0f) ? q->w
              : !Float_ApproximatelyEqual(q->z, 0.0f) ? q->z
              : !Float_ApproximatelyEqual(q->y, 0.0f) ? q->y
              : !Float_ApproximatelyEqual(q->x, 0.0f) ? q->x
              : 0.0f;

  if (value < 0.0f) {
    out->x = -q->x;
    out->y = -q->y;
    out->z = -q->z;
    out->w = -q->w;
  } else {
    out->x = q->x;
    out->y = q->y;
    out->z = q->z;
    out->w = q->w;
  }
  Assert(Quat_Validate(out) == Error_None);
}

void Quat_ICanonicalize (Quat* q) {
  float value = !Float_ApproximatelyEqual(q->w, 0.0f) ? q->w
              : !Float_ApproximatelyEqual(q->z, 0.0f) ? q->z
              : !Float_ApproximatelyEqual(q->y, 0.0f) ? q->y
              : !Float_ApproximatelyEqual(q->x, 0.0f) ? q->x
              : 0.0f;

  if (value < 0.0f) {
    q->x = -q->x;
    q->y = -q->y;
    q->z = -q->z;
    q->w = -q->w;
  }
  Assert(Quat_Validate(q) == Error_None);
}

float Quat_Dot (Quat const* q, Quat const* p) {
  return q->x*p->x
       + q->y*p->y
       + q->z*p->z
       + q->w*p->w;
}

bool Quat_Equal (Quat const* q, Quat const* p) {
  Quat cq; Quat_Canonicalize(q, &cq);
  Quat cp; Quat_Canonicalize(p, &cp);
  return cq.x == cp.x
      && cq.y == cp.y
      && cq.z == cp.z
      && cq.w == cp.w;
}

bool Quat_ApproximatelyEqual (Quat const* q, Quat const* p) {
  Quat cq; Quat_Canonicalize(q, &cq);
  Quat cp; Quat_Canonicalize(p, &cp);
  return Abs(cq.x - cp.x) < 1e-3f
      && Abs(cq.y - cp.y) < 1e-3f
      && Abs(cq.z - cp.z) < 1e-3f
      && Abs(cq.w - cp.w) < 1e-3f;
}

void Quat_Inverse (Quat const* q, Quat* out) {
  float magSq = q->x*q->x
              + q->y*q->y
              + q->z*q->z
              + q->w*q->w;
  Assert(magSq > 1e-12f);
  out->x = -q->x / magSq,
  out->y = -q->y / magSq,
  out->z = -q->z / magSq,
  out->w =  q->w / magSq;
  Assert(Quat_Validate(out) == Error_None);
}

void Quat_IInverse (Quat* q) {
  float magSq = q->x*q->x
              + q->y*q->y
              + q->z*q->z
              + q->w*q->w;
  Assert(magSq > 1e-12f);
  q->x = -q->x/magSq;
  q->y = -q->y/magSq;
  q->z = -q->z/magSq;
  q->w =  q->w/magSq;
  Assert(Quat_Validate(q) == Error_None);
}

void Quat_Lerp (Quat const* q, Quat const* p, float t, Quat* out) {
  float d = Quat_Dot(p, q);
  Quat dp;
  if (d < 0.0f) {
    dp.x = -p->x;
    dp.y = -p->y;
    dp.z = -p->z;
    dp.w = -p->w;
  } else {
    dp = *p;
  }

  float x = q->x + (dp.x - q->x) * t;
  float y = q->y + (dp.y - q->y) * t;
  float z = q->z + (dp.z - q->z) * t;
  float w = q->w + (dp.w - q->w) * t;
  float rcpMag = 1.0f / Sqrt(x*x + y*y + z*z + w*w);
  out->x = x*rcpMag;
  out->y = y*rcpMag;
  out->z = z*rcpMag;
  out->w = w*rcpMag;
  Assert(Quat_Validate(out) == Error_None);
}

void Quat_ILerp (Quat* q, Quat const* p, float t) {
  float d = Quat_Dot(p, q);
  Quat dp;
  if (d < 0.0f) {
    dp.x = -p->x,
    dp.y = -p->y,
    dp.z = -p->z,
    dp.w = -p->w;
  } else {
    dp = *p;
  }

  float x = q->x + (dp.x - q->x) * t;
  float y = q->y + (dp.y - q->y) * t;
  float z = q->z + (dp.z - q->z) * t;
  float w = q->w + (dp.w - q->w) * t;
  float rcpMag = 1.0f / Sqrt(x*x + y*y + z*z + w*w);
  q->x = x*rcpMag;
  q->y = y*rcpMag;
  q->z = z*rcpMag;
  q->w = w*rcpMag;
  Assert(Quat_Validate(q) == Error_None);
}

/* TODO : The Lua side implementation of this looks much cleaner.
  return (Quat(
    a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
    a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
    a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
    a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z))
*/
void Quat_Mul (Quat const* q, Quat const* p, Quat* out) {
  Vec3f qv = { q->x, q->y, q->z };
  Vec3f pv = { p->x, p->y, p->z };
  Vec3f rv = Vec3f_Add(
    Vec3f_Add(
      Vec3f_Muls(qv, p->w),
      Vec3f_Muls(pv, q->w)),
    Vec3f_Cross(qv, pv));
  out->x = rv.x;
  out->y = rv.y;
  out->z = rv.z;
  out->w = q->w*p->w - Vec3f_Dot(qv, pv);
  Assert(Quat_Validate(out) == Error_None);
}

void Quat_IMul (Quat* q, Quat const* p) {
  Vec3f qv = { q->x, q->y, q->z };
  Vec3f pv = { p->x, p->y, p->z };
  Vec3f rv = Vec3f_Add(
    Vec3f_Add(
      Vec3f_Muls(qv, p->w),
      Vec3f_Muls(pv, q->w)),
    Vec3f_Cross(qv, pv));
  q->x = rv.x;
  q->y = rv.y;
  q->z = rv.z;
  q->w = q->w*p->w - Vec3f_Dot(qv, pv);
  Assert(Quat_Validate(q) == Error_None);
}

void Quat_MulV (Quat const* q, Vec3f const* v, Vec3f* out) {
  Vec3f u = { q->x, q->y, q->z };
  float w = q->w;
  Vec3f t = Vec3f_Cross(u, *v);
  *out = Vec3f_Add(
    Vec3f_Muls(u, 2.0f * Vec3f_Dot(u, *v)),
    Vec3f_Add(
      Vec3f_Muls(*v, 2.0f * w * w - 1.0f),
      Vec3f_Muls(t, 2.0f * w)
    )
  );
  Assert(Vec3f_Validate(*out) == Error_None);
}

void Quat_Normalize (Quat const* q, Quat* out) {
  float mag = Sqrt(q->x*q->x + q->y*q->y + q->z*q->z + q->w*q->w);
  Assert(mag != 0.0f);
  out->x = q->x / mag;
  out->y = q->y / mag;
  out->z = q->z / mag;
  out->w = q->w / mag;
  Assert(Quat_Validate(q) == Error_None);
}

void Quat_INormalize (Quat* q) {
  float mag = Sqrt(q->x*q->x + q->y*q->y + q->z*q->z + q->w*q->w);
  Assert(mag != 0.0f);
  q->x /= mag;
  q->y /= mag;
  q->z /= mag;
  q->w /= mag;
  Assert(Quat_Validate(q) == Error_None);
}

void Quat_Scale (Quat const* q, float scale, Quat* out) {
  out->x = scale * q->x;
  out->y = scale * q->y;
  out->z = scale * q->z;
  out->w = scale * q->w;
  Assert(Quat_Validate(out) == Error_None);
}

void Quat_IScale (Quat* q, float scale) {
  q->x *= scale;
  q->y *= scale;
  q->z *= scale;
  q->w *= scale;
  Assert(Quat_Validate(q) == Error_None);
}

/* TODO : There was a normalized q in Quat.ext, but it wasn't used. Is this correct? */
void Quat_Slerp (Quat const* q, Quat const* p, float t, Quat* out) {
  Quat np; Quat_Normalize(p, &np);
  float d = Quat_Dot(q, p);
  if (d < 0.0f) {
    np.x = -np.x;
    np.y = -np.y;
    np.z = -np.z;
    np.w = -np.w;
    d = -d;
  }

  if (d > 0.9995f) {
    Quat_Lerp(q, p, t, out);
    Assert(Quat_Validate(out) == Error_None);
    return;
  }

  d = ClampUnit(d);
  float angle = t * Acos(d);
  Quat c = {
    p->x - d*q->x,
    p->y - d*q->y,
    p->z - d*q->z,
    p->w - d*q->w
  };
  Quat_INormalize(&c);

  float fa = Cos(angle);
  float fc = Sin(angle);
  out->x = fa*q->x + fc*c.x;
  out->y = fa*q->y + fc*c.y;
  out->z = fa*q->z + fc*c.z;
  out->w = fa*q->w + fc*c.w;
  Assert(Quat_Validate(out) == Error_None);
}

/* TODO : There was a normalized q in Quat.ext, but it wasn't used. Is this correct? */
void Quat_ISlerp (Quat* q, Quat const* p, float t) {
  Quat np; Quat_Normalize(p, &np);
  float d = Quat_Dot(q, p);
  if (d < 0.0f) {
    np.x = -np.x;
    np.y = -np.y;
    np.z = -np.z;
    np.w = -np.w;
    d = -d;
  }

  if (d > 0.9995f) {
    Quat_ILerp(q, p, t);
    Assert(Quat_Validate(q) == Error_None);
    return;
  }

  d = ClampUnit(d);
  float angle = t * Acos(d);
  Quat c = {
    p->x - d*q->x,
    p->y - d*q->y,
    p->z - d*q->z,
    p->w - d*q->w
  };
  Quat_INormalize(&c);

  float fa = Cos(angle);
  float fc = Sin(angle);
  q->x = fa*q->x + fc*c.x;
  q->y = fa*q->y + fc*c.y;
  q->z = fa*q->z + fc*c.z;
  q->w = fa*q->w + fc*c.w;
  Assert(Quat_Validate(q) == Error_None);
}

cstr Quat_ToString (Quat const* q) {
  static char buffer[512];
  snprintf(buffer, (size_t) Array_GetSize(buffer),
    "(%.4f, %.4f, %.4f, %.4f)",
    q->x, q->y, q->z, q->w);
  return buffer;
}

Error Quat_Validate(Quat const* q) {
  Error e = Error_None;
  e |= Float_Validate(q->x);
  e |= Float_Validate(q->y);
  e |= Float_Validate(q->z);
  e |= Float_Validate(q->w);
  return e;
}

void Quat_FromAxisAngle (Vec3f const* axis, float radians, Quat* out) {
  radians *= 0.5f;
  Vec3f v = Vec3f_Muls(*axis, Sin(radians));
  out->x = v.x;
  out->y = v.y;
  out->z = v.z;
  out->w = Cos(radians);
  Assert(Quat_Validate(out) == Error_None);
}

void Quat_FromBasis (Vec3f const* x, Vec3f const* y, Vec3f const* z, Quat* out) {
  float r = x->x + y->y + z->z;
  if (r > 0.0f) {
    out->w = Sqrt(r + 1.0f) * 0.5f;
    float w4 = 1.0f / (4.0f * out->w);
    out->x = (y->z - z->y) * w4;
    out->y = (z->x - x->z) * w4;
    out->z = (x->y - y->x) * w4;

  } else if (x->x > y->y && x->x > z->z) {
    out->x = Sqrt(1.0f + x->x - y->y - z->z) * 0.5f;
    float x4 = 1.0f / (4.0f * out->x);
    out->y = (y->x + x->y) * x4;
    out->z = (z->x + x->z) * x4;
    out->w = (y->z - z->y) * x4;

  } else if (y->y > z->z) {
    out->y = Sqrt(1.0f + y->y - x->x - z->z) * 0.5f;
    float y4 = 1.0f / (4.0f * out->y);
    out->x = (y->x + x->y) * y4;
    out->z = (z->y + y->z) * y4;
    out->w = (z->x - x->z) * y4;

  } else {
    out->z = Sqrt(1.0f + z->z - x->x - y->y) * 0.5f;
    float z4 = 1.0f / (4.0f * out->z);
    out->x = (z->x + x->z) * z4;
    out->y = (z->y + y->z) * z4;
    out->w = (x->y - y->x) * z4;
  }
  Assert(Quat_Validate(out) == Error_None);
}

void Quat_FromLookUp (Vec3f const* look, Vec3f const* up, Quat* out) {
  Vec3f z = Vec3f_Normalize(Vec3f_Muls(*look, -1.0f));
  Vec3f x = Vec3f_Normalize(Vec3f_Cross(*up, z));
  Vec3f y = Vec3f_Cross(z, x);
  Quat_FromBasis(&x, &y, &z, out);
}

void Quat_FromRotateTo (Vec3f const* from, Vec3f const* to, Quat* out) {
  Vec3f axis = Vec3f_Cross(
    Vec3f_Normalize(*from),
    Vec3f_Normalize(*to)
  );
  float angle = Asin(Vec3f_Length(axis));
  Quat_FromAxisAngle(&axis, angle, out);
}

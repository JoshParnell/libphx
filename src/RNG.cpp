#include "Hash.h"
#include "PhxMemory.h"
#include "RNG.h"
#include "PhxMath.h"
#include "Random.h"
#include "Quat.h"
#include "PhxString.h"
#include "TimeStamp.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

/* TODO : Rigorous testing of Xoroshiro128 & my implementation thereof. */

struct RNG {
  uint64 seed;
  uint64 state[2];
};

inline static uint64 RNG_Next64 (RNG* self) {
	return Random_Xoroshiro128(self->state[0], self->state[1]);
}

inline static uint32 RNG_Next32 (RNG* self) {
  return (uint32)(RNG_Next64(self) & 0xFFFFFFFFU);
}

/* Seed Xoroshiro with SM64. */
inline static void RNG_Init (RNG* self) {
  uint64 seed = self->seed;
  /* TODO : Using seed as both rolling output and seed buffer is not how SM64 is
   *        intended to work and is likely causing degenerate output space.
   *        Rigorous investigation needed. */
  for (int i = 0; i < 64; ++i)
    seed = Random_SplitMix64(seed);
  self->state[0] = Random_SplitMix64(seed);
  self->state[1] = Random_SplitMix64(seed);
  for (int i = 0; i < 64; ++i)
    RNG_Next64(self);
}

RNG* RNG_Create (uint64 seed) {
  RNG* self = MemNew(RNG);
  self->seed = seed;
  RNG_Init(self);
  return self;
}

RNG* RNG_FromStr (cstr s) {
  return RNG_Create(Hash_XX64(s, StrLen(s), 0));
}

RNG* RNG_FromTime () {
  return RNG_Create(TimeStamp_Get());
}

void RNG_Free (RNG* self) {
  MemFree(self);
}

void RNG_Rewind (RNG* self) {
  RNG_Init(self);
}

bool RNG_Chance (RNG* self, double probability) {
  return RNG_GetUniform(self) < probability;
}

int32 RNG_Get31 (RNG* self) {
  uint32 i = RNG_Next32(self) & 0x7FFFFFFFU;
  return *(int32*)&i;
}

uint32 RNG_Get32 (RNG* self) {
  return RNG_Next32(self);
}

uint64 RNG_Get64 (RNG* self) {
  return RNG_Next64(self);
}

double RNG_GetAngle (RNG* self) {
  return Tau * RNG_GetUniform(self);
}

int RNG_GetInt (RNG* self, int lower, int upper) {
  double t = RNG_GetUniform(self);
  return int(Round(lower + t * (upper - lower)));
}

RNG* RNG_GetRNG (RNG* self) {
  return RNG_Create(RNG_Get64(self));
}

double RNG_GetUniform (RNG* self) {
  return ldexp((double)RNG_Next32(self), -32);
}

double RNG_GetUniformRange (RNG* self, double lower, double upper) {
  double t = ldexp((double)RNG_Next32(self), -32);
  return lower + t * (upper - lower);
}

double RNG_GetErlang (RNG* self, int k) {
  double sum = 0.0;
  for (int i = 0; i < k; ++i)
    sum += RNG_GetExp(self);
  return sum / (double)k;
}

double RNG_GetExp (RNG* self) {
  return -Log(Max(1.0 - RNG_GetUniform(self), F64_EPSILON));
}

double RNG_GetGaussian (RNG* self) {
  double angle = RNG_GetAngle(self);
  double radius = 1.0 - RNG_GetUniform(self);
  return Cos(angle) * Sqrt(-2.0 * Log(radius));
}

void RNG_GetAxis2 (RNG* self, Vec2f* out) {
  *out = Vec2f_Create(0.0f, 0.0f);
  int axis = RNG_GetInt(self, 0, 3);
  switch (axis) {
    case 0: out->x =  1.0f; return;
    case 1: out->x = -1.0f; return;
    case 2: out->y =  1.0f; return;
    case 3: out->y = -1.0f; return;
  }
}

void RNG_GetAxis3 (RNG* self, Vec3f* out) {
  *out = Vec3f_Create(0.0f, 0.0f, 0.0f);
  int axis = RNG_GetInt(self, 0, 5);
  switch (axis) {
    case 0: out->x =  1.0f; return;
    case 1: out->x = -1.0f; return;
    case 2: out->y =  1.0f; return;
    case 3: out->y = -1.0f; return;
    case 4: out->z =  1.0f; return;
    case 5: out->z = -1.0f; return;
  }
}

void RNG_GetDir2 (RNG* self, Vec2f* out) {
  double angle = RNG_GetAngle(self);
  *out = Vec2f_Create((float)Cos(angle), (float)Sin(angle));
}

void RNG_GetDir3 (RNG* self, Vec3f* out) {
  for (;;) {
    double x = 2.0 * RNG_GetUniform(self) - 1.0;
    double y = 2.0 * RNG_GetUniform(self) - 1.0;
    double z = 2.0 * RNG_GetUniform(self) - 1.0;
    double m2 = x*x + y*y + z*z;
    if (m2 <= 1.0 && m2 > 1e-6) {
      m2 = Sqrt(m2);
      out->x = (float)(x / m2);
      out->y = (float)(y / m2);
      out->z = (float)(z / m2);
      return;
    }
  }
}

/* NOTE : Disc & Sphere use rejection-sampling. For 2D and 3D unit balls,
 *        rejection is very fast. Although not profiled, I suspect the
 *        trigonometric equivalents (i.e. analytic sampling requiring no loop)
 *        are actually slower. */

void RNG_GetDisc (RNG* self, Vec2f* out) {
  for (;;) {
    double x = 2.0 * RNG_GetUniform(self) - 1.0;
    double y = 2.0 * RNG_GetUniform(self) - 1.0;
    if ((x*x + y*y) <= 1.0) {
      out->x = (float)x;
      out->y = (float)y;
      return;
    }
  }
}

double RNG_GetSign (RNG* self) {
  return RNG_GetUniform(self) > 0.5 ? 1.0 : -1.0;
}

void RNG_GetSphere (RNG* self, Vec3f* out) {
  for (;;) {
    double x = 2.0 * RNG_GetUniform(self) - 1.0;
    double y = 2.0 * RNG_GetUniform(self) - 1.0;
    double z = 2.0 * RNG_GetUniform(self) - 1.0;
    if ((x*x + y*y + z*z) <= 1.0) {
      out->x = (float)x;
      out->y = (float)y;
      out->z = (float)z;
      return;
    }
  }
}

void RNG_GetVec2 (RNG* self, Vec2f* out, double lower, double upper) {
  out->x = (float)RNG_GetUniformRange(self, lower, upper);
  out->y = (float)RNG_GetUniformRange(self, lower, upper);
}

void RNG_GetVec3 (RNG* self, Vec3f* out, double lower, double upper) {
  out->x = (float)RNG_GetUniformRange(self, lower, upper);
  out->y = (float)RNG_GetUniformRange(self, lower, upper);
  out->z = (float)RNG_GetUniformRange(self, lower, upper);
}

void RNG_GetVec4 (RNG* self, Vec4f* out, double lower, double upper) {
  out->x = (float)RNG_GetUniformRange(self, lower, upper);
  out->y = (float)RNG_GetUniformRange(self, lower, upper);
  out->z = (float)RNG_GetUniformRange(self, lower, upper);
  out->w = (float)RNG_GetUniformRange(self, lower, upper);
}

/* http://marc-b-reynolds.github.io/distribution/2017/01/27/UniformRot.html */
void RNG_GetQuat (RNG* self, Quat* out) {
  Vec2f p0, p1;
  RNG_GetDisc(self, &p0);
  RNG_GetDisc(self, &p1);
  double d0 = Vec2f_LengthSquared(p0);
  double d1 = Vec2f_LengthSquared(p1) + F64_EPSILON;
  double s = Sqrt((1.0 - d0) / d1);
  out->x = (float)p0.y;
  out->y = (float)(p1.x * s);
  out->z = (float)(p1.y * s);
  out->w = (float)p0.x;
}

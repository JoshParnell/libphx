#ifndef PHX_RNG
#define PHX_RNG

#include "Common.h"

/* --- RNG ---------------------------------------------------------------------
 *
 *   A 64-bit random number generator with convenience methods for generating
 *   a wide variety of random objects and distributions.
 *
 *   RNGs are lightweight, using the Xoroshiro128 algorithm for high-quality
 *   random numbers with high performance and a small internal state. As such,
 *   they can be created and destroyed fairly liberally.
 *
 *   Distribution functions:
 *
 *     RNG_Chance          : True with the given probability (in [0, 1])
 *     RNG_Get31           : Uniform non-negative 32-bit signed integer
 *     RNG_GetAngle        : Uniform angle in [0, 2Pi]
 *     RNG_GetErlang       : Sum of k exponentially-distributed variables
 *     RNG_GetExp          : Exponentially-distributed random variable
 *     RNG_GetGaussian     : Gaussian-distributed random variable with mean 0,
 *                           variance 1
 *     RNG_GetRNG          : Equivalent to RNG_Create(RNG_Get64(self))
 *     RNG_GetUniform      : Uniform double in [0, 1] (inclusive!)
 *     RNG_GetUniformRange : Uniform double in [lower, upper]
 *     RNG_GetAxis2        : {(+-1, 0), (0, +-1)}
 *     RNG_GetAxis3        : {(+-1, 0, 0), (0, +-1, 0), (0, 0, +-1)}
 *     RNG_GetDir2         : Uniform Vec2 with unit length
 *     RNG_GetDir3         : Uniform Vec3 with unit length
 *     RNG_GetDisc         : Uniform Vec2 inside the unit disc
 *     RNG_GetSign         : {-1, 1} with equal probability
 *     RNG_GetSphere       : Uniform Vec3 inside the unit sphere
 *     RNG_GetQuat         : Uniform orientation in 3D space
 *
 * -------------------------------------------------------------------------- */

PHX_API RNG*    RNG_Create           (uint64 seed);
PHX_API RNG*    RNG_FromStr          (cstr);
PHX_API RNG*    RNG_FromTime         ();
PHX_API void    RNG_Free             (RNG*);
PHX_API void    RNG_Rewind           (RNG*);

PHX_API bool    RNG_Chance           (RNG*, double probability);
PHX_API  int32  RNG_Get31            (RNG*);
PHX_API uint32  RNG_Get32            (RNG*);
PHX_API uint64  RNG_Get64            (RNG*);
PHX_API double  RNG_GetAngle         (RNG*);
PHX_API double  RNG_GetErlang        (RNG*, int k);
PHX_API double  RNG_GetExp           (RNG*);
PHX_API double  RNG_GetGaussian      (RNG*);
PHX_API int     RNG_GetInt           (RNG*, int lower, int upper);
PHX_API RNG*    RNG_GetRNG           (RNG*);
PHX_API double  RNG_GetSign          (RNG*);
PHX_API double  RNG_GetUniform       (RNG*);
PHX_API double  RNG_GetUniformRange  (RNG*, double lower, double upper);

PHX_API void    RNG_GetAxis2         (RNG*, Vec2f* out);
PHX_API void    RNG_GetAxis3         (RNG*, Vec3f* out);

PHX_API void    RNG_GetDir2          (RNG*, Vec2f* out);
PHX_API void    RNG_GetDir3          (RNG*, Vec3f* out);
PHX_API void    RNG_GetDisc          (RNG*, Vec2f* out);
PHX_API void    RNG_GetSphere        (RNG*, Vec3f* out);

PHX_API void    RNG_GetVec2          (RNG*, Vec2f* out, double lower, double upper);
PHX_API void    RNG_GetVec3          (RNG*, Vec3f* out, double lower, double upper);
PHX_API void    RNG_GetVec4          (RNG*, Vec4f* out, double lower, double upper);

PHX_API void    RNG_GetQuat          (RNG*, Quat* out);

#endif

#ifndef PHX_SoundDef
#define PHX_SoundDef

#include "Common.h"
#include "RefCounted.h"
#include "fmod/fmod.h"

struct SoundDesc {
  RefCounted;
  FMOD_SOUND* handle;
  cstr        name;
  cstr        path;
};

typedef uint8 SoundState;
const SoundState SoundState_Null     = 0;
const SoundState SoundState_Loading  = 1;
const SoundState SoundState_Paused   = 2;
const SoundState SoundState_Playing  = 3;
const SoundState SoundState_Finished = 4;
const SoundState SoundState_Freed    = 5;

struct Sound {
  SoundDesc*    desc;
  FMOD_CHANNEL* handle;
  SoundState    state;
  Vec3f const*  autoPos;
  Vec3f const*  autoVel;
  bool          freeOnFinish;
};

#endif

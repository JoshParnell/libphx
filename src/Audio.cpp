#include "ArrayList.h"
#include "Audio.h"
#include "FMODError.h"
#include "MemPool.h"
#include "PhxMath.h"
#include "Sound.h"
#include "SoundDef.h"
#include "StrMap.h"
#include "Vec3.h"
#include "fmod/fmod.h"

#define AUDIO_CHANNELS 1024
#define SOUNDPOOL_BLOCK_SIZE 128

struct Audio {
  FMOD_SYSTEM*      handle;
  StrMap*           descMap;
  MemPool*          soundPool;
  ArrayList(Sound*, playingSounds);
  ArrayList(Sound*, freeingSounds);

  Vec3f const* autoPos;
  Vec3f const* autoVel;
  Vec3f const* autoFwd;
  Vec3f const* autoUp;
} static self;

void Audio_Init () {
    /* Initialize Debugging. */ {
      FMOD_DEBUG_FLAGS flags = 0;
      NCHECK(flags |= FMOD_DEBUG_LEVEL_NONE);
      CHECK1(flags |= FMOD_DEBUG_LEVEL_ERROR);
      CHECK1(flags |= FMOD_DEBUG_LEVEL_WARNING);
      CHECK2(flags |= FMOD_DEBUG_LEVEL_LOG);

      FMOD_RESULT res;
      res = FMOD_Debug_Initialize(flags, FMOD_DEBUG_MODE_FILE, 0, "log/fmod.txt");
      if (res != FMOD_OK && res != FMOD_ERR_UNSUPPORTED)
        FMODCALL(res);
    }

  /* Initialize FMOD. */ {
    FMODCALL(FMOD_System_Create(&self.handle));

    uint version;
    FMODCALL(FMOD_System_GetVersion(self.handle, &version));
    if (version < FMOD_VERSION)
      Fatal("Audio_Create: FMOD library link/compile version mismatch");

    /* NOTE : The fake HRTF mentioned in FMOD_INIT_CHANNEL_LOWPASS and
     *        FMOD_ADVANCEDSETTINGS has been removed from FMOD.
     *        http://www.fmod.org/questions/question/hrtf-does-not-appear-to-work/ */

    FMOD_INITFLAGS flags = 0;
    flags |= FMOD_INIT_NORMAL;
    flags |= FMOD_INIT_3D_RIGHTHANDED;
    flags |= FMOD_INIT_CHANNEL_DISTANCEFILTER;
    CHECK2(flags |= FMOD_INIT_PROFILE_ENABLE);
    FMODCALL(FMOD_System_Init(self.handle, AUDIO_CHANNELS, flags, 0));
  }

  /* Initialize audio instance data. */ {
    self.descMap = StrMap_Create(128);
    self.soundPool = MemPool_Create(sizeof(Sound), SOUNDPOOL_BLOCK_SIZE);
  }
}

void Audio_Free () {
  FMODCALL(FMOD_System_Release(self.handle));
  StrMap_Free(self.descMap);
  MemPool_Free(self.soundPool);
  ArrayList_Free(self.playingSounds);
  ArrayList_Free(self.freeingSounds);
}

void Audio_AttachListenerPos (Vec3f const* pos, Vec3f const* vel, Vec3f const* fwd, Vec3f const* up) {
  self.autoPos = pos;
  self.autoVel = vel;
  self.autoFwd = fwd;
  self.autoUp  = up;
  Audio_SetListenerPos(pos, vel, fwd, up);
}

void Audio_Set3DSettings (float doppler, float scale, float rolloff) {
  FMODCALL(FMOD_System_Set3DSettings(self.handle, doppler, scale, rolloff));
}

void Audio_SetListenerPos (
  Vec3f const* pos,
  Vec3f const* vel,
  Vec3f const* fwd,
  Vec3f const* up)
{
  Assert(sizeof(*pos) == sizeof(FMOD_VECTOR));
  Assert(!fwd || Approx(Vec3f_Length(*fwd), 1));
  Assert(!up || Approx(Vec3f_Length(*up), 1));
  Assert(!fwd || !up || Approx(Vec3f_Dot(*fwd, *up), 0));

  FMODCALL(FMOD_System_Set3DListenerAttributes(
    self.handle,
    0,
    (FMOD_VECTOR*) pos,
    (FMOD_VECTOR*) vel,
    (FMOD_VECTOR*) fwd,
    (FMOD_VECTOR*) up
  ));
}

void Audio_Update () {
  FMODCALL(FMOD_System_Update(self.handle));
  Audio_SetListenerPos(self.autoPos, self.autoVel, self.autoFwd, self.autoUp);

  ArrayList_ForEachI(self.playingSounds, i) {
    Sound* sound = ArrayList_Get(self.playingSounds, i);
    /* TODO : Refine the API to make this less awkward */
    if (!Sound_IsFreed(sound) && Sound_IsPlaying(sound)) {
      Sound_Update(sound);
    } else {
      ArrayList_RemoveAtFast(self.playingSounds, i--);
    }
  }

  ArrayList_ForEachI(self.freeingSounds, i) {
    Sound* sound = ArrayList_Get(self.freeingSounds, i);
    Audio_DeallocSound(sound);
  }
  ArrayList_Clear(self.freeingSounds);
}

int32 Audio_GetLoadedCount () {
  uint32 size = StrMap_GetSize(self.descMap);
  Assert(size <= INT32_MAX);
  return (int32) size;
}

int32 Audio_GetPlayingCount () {
  return ArrayList_GetSize (self.playingSounds);
}

int32 Audio_GetTotalCount () {
  uint32 size = MemPool_GetSize(self.soundPool);
  Assert(size <= INT32_MAX);
  return (int32) size;
}

void* Audio_GetHandle () {
  return self.handle;
}

SoundDesc* Audio_AllocSoundDesc (cstr name) {
  SoundDesc* desc = (SoundDesc*) StrMap_Get(self.descMap, name);
  if (!desc) {
    desc = MemNewZero(SoundDesc);
    StrMap_Set(self.descMap, name, desc);
  }
  return desc;
}

void Audio_DeallocSoundDesc (SoundDesc* desc) {
  StrMap_Remove(self.descMap, desc->name);
  MemFree(desc);
}

Sound* Audio_AllocSound () {
  return (Sound*) MemPool_Alloc(self.soundPool);
}

void Audio_DeallocSound (Sound* sound) {
  MemPool_Dealloc(self.soundPool, sound);
}

void Audio_SoundStateChanged (Sound* sound) {
  if (Sound_IsFreed(sound)) {
    ArrayList_Append(self.freeingSounds, sound);
  } else if (Sound_IsPlaying(sound)) {
    ArrayList_Append(self.playingSounds, sound);

    CHECK1(
      if (ArrayList_GetSize(self.playingSounds) == AUDIO_CHANNELS + 1)
        Warn("Audio: Exceeded the number of available sound channels (%i)", AUDIO_CHANNELS);
    )
  }
}

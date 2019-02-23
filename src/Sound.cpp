#include "Audio.h"
#include "Bytes.h"
#include "FMODError.h"
#include "PhxMemory.h"
#include "PhxMath.h"
#include "Sound.h"
#include "SoundDesc.h"
#include "SoundDef.h"
#include "Vec3.h"
#include "fmod/fmod.h"

static void Sound_SetState(Sound*, SoundState);

static FMOD_RESULT F_CALLBACK Sound_Callback (
  FMOD_CHANNELCONTROL*              channel,
  FMOD_CHANNELCONTROL_TYPE          controlType,
  FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType,
  void*, void*)
{
  if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END) {
    Assert(controlType == FMOD_CHANNELCONTROL_CHANNEL);
    Sound* self;
    FMODCALL(FMOD_Channel_GetUserData((FMOD_CHANNEL*) channel, (void**) &self));
    Sound_SetState(self, SoundState_Finished);
  }

  return FMOD_OK;
}

inline static void Sound_EnsureLoadedImpl (Sound* self, cstr func) {
  if (self->state == SoundState_Loading) {
    SoundDesc_FinishLoad(self->desc, func);
    FMODCALL(FMOD_System_PlaySound((FMOD_SYSTEM*) Audio_GetHandle(), self->desc->handle, 0, true, &self->handle));
    FMODCALL(FMOD_Channel_SetUserData(self->handle, self));
    FMODCALL(FMOD_Channel_SetCallback(self->handle, Sound_Callback));
    Sound_SetState(self, SoundState_Paused);

    if (Sound_Get3D(self)) {
      Vec3f zero = { 0, 0, 0 };
      Sound_Set3DPos(self, &zero, &zero);
    }
  }
}
#define Sound_EnsureLoaded(...) Sound_EnsureLoadedImpl(__VA_ARGS__, __func__)

inline static void Sound_EnsureNotFreedImpl (Sound* self, cstr func) {
  if (self->state == SoundState_Freed) {
    cstr name = (self->desc->_refCount > 0) ? self->desc->name : "<SoundDesc has been freed>";
    Fatal("%s: Sound has been freed.\n  Name: %s", func, name);
  }
}
#define Sound_EnsureNotFreed(...) Sound_EnsureNotFreedImpl(__VA_ARGS__, __func__)

inline static void Sound_EnsureStateImpl (Sound* self, cstr func) {
  Sound_EnsureLoadedImpl(self, func);
  Sound_EnsureNotFreedImpl(self, func);
}
#define Sound_EnsureState(...) Sound_EnsureStateImpl(__VA_ARGS__, __func__)

static void Sound_SetState (Sound* self, SoundState nextState) {
  if (nextState == self->state) return;

  switch (nextState) {
    default: Fatal("Sound_SetState: Unhandled case: %i", nextState);

    case SoundState_Loading:
      Assert(self->state == SoundState_Null);
      break;

    case SoundState_Playing:
      FMODCALL(FMOD_Channel_SetPaused(self->handle, false));
      break;

    case SoundState_Paused:
      FMODCALL(FMOD_Channel_SetPaused(self->handle, true));
      break;

    case SoundState_Finished:
      FMODCALL(FMOD_Channel_Stop(self->handle));
      break;

    case SoundState_Freed:
      /* NOTE : Deallocation is deferred to Audio_Update for performance. */
      break;
  }

  self->state = nextState;
  Audio_SoundStateChanged(self);

  if (self->freeOnFinish && self->state == SoundState_Finished)
    Sound_Free(self);
}

static Sound* Sound_Create (cstr name, bool immediate, bool isLooped, bool is3D) {
  SoundDesc* desc = SoundDesc_Load(name, immediate, isLooped, is3D);
  Sound* self = Audio_AllocSound();
  self->desc = desc;
  Sound_SetState(self, SoundState_Loading);
  return self;
}

Sound* Sound_Load (cstr name, bool isLooped, bool is3D) {
  Sound* self = Sound_Create(name, true, isLooped, is3D);
  Sound_EnsureLoaded(self);
  return self;
}

Sound* Sound_LoadAsync (cstr name, bool isLooped, bool is3D) {
  Sound* self = Sound_Create(name, false, isLooped, is3D);
  return self;
}

Sound* Sound_Clone (Sound* self) {
  Sound_EnsureState(self);
  Sound* clone = Audio_AllocSound();
  *clone = *self;
  SoundDesc_Acquire(self->desc);
  clone->handle = 0;
  clone->state = SoundState_Null;
  Sound_SetState(clone, SoundState_Loading);
  return clone;
}

void Sound_ToFile (Sound* self, cstr name) {
  Sound_EnsureState(self);
  SoundDesc_ToFile(self->desc, name);
}

void Sound_Acquire (Sound* self) {
  Sound_EnsureState(self);
  RefCounted_Acquire(self->desc);
}

void Sound_Free (Sound* self) {
  Sound_EnsureState(self);
  Sound_SetState(self, SoundState_Finished);
  Sound_SetState(self, SoundState_Freed);
  SoundDesc_Free(self->desc);
}

void Sound_Play (Sound* self) {
  Sound_EnsureState(self);
  Sound_SetState(self, SoundState_Playing);
}

void Sound_Pause (Sound* self) {
  Sound_EnsureState(self);
  Sound_SetState(self, SoundState_Paused);
}

void Sound_Rewind (Sound* self) {
  Sound_EnsureState(self);
  FMODCALL(FMOD_Channel_SetPosition(self->handle, 0, FMOD_TIMEUNIT_PCM));
}

bool Sound_Get3D (Sound* self) {
  Sound_EnsureState(self);
  FMOD_MODE mode;
  FMODCALL(FMOD_Channel_GetMode(self->handle, &mode));
  return (mode & FMOD_3D) == FMOD_3D;
}

float Sound_GetDuration (Sound* self) {
  Sound_EnsureState(self);
  return SoundDesc_GetDuration(self->desc);
}

bool Sound_GetLooped (Sound* self) {
  Sound_EnsureState(self);
  FMOD_MODE mode;
  FMODCALL(FMOD_Channel_GetMode(self->handle, &mode));
  return (mode & FMOD_LOOP_NORMAL) == FMOD_LOOP_NORMAL;
}

cstr Sound_GetName (Sound* self) {
  Sound_EnsureNotFreed(self);
  return SoundDesc_GetName(self->desc);
}

cstr Sound_GetPath (Sound* self) {
  Sound_EnsureNotFreed(self);
  return SoundDesc_GetPath(self->desc);
}

bool Sound_IsFinished (Sound* self) {
  return self->state == SoundState_Finished;
}

bool Sound_IsPlaying (Sound* self) {
  return self->state == SoundState_Playing;
}

void Sound_Attach3DPos (Sound* self, Vec3f const* pos, Vec3f const* vel) {
  //EnsureState happens in Set3DPos already
  Sound_Set3DPos(self, pos, vel);
  self->autoPos = pos;
  self->autoVel = vel;
}

void Sound_Set3DLevel (Sound* self, float level) {
  Sound_EnsureState(self);
  FMODCALL(FMOD_Channel_Set3DLevel(self->handle, level));
}

void Sound_Set3DPos (Sound* self, Vec3f const* pos, Vec3f const* vel) {
  Sound_EnsureState(self);
  Assert(sizeof(*pos) == sizeof(FMOD_VECTOR));
  FMODCALL(FMOD_Channel_Set3DAttributes(
    self->handle, (FMOD_VECTOR*) pos, (FMOD_VECTOR*) vel, 0
  ));
}

void Sound_SetFreeOnFinish (Sound* self, bool freeOnFinish) {
  self->freeOnFinish = freeOnFinish;
}

void Sound_SetPan (Sound* self, float pan) {
  Sound_EnsureState(self);
  FMODCALL(FMOD_Channel_SetPan(self->handle, pan));
}

void Sound_SetPitch (Sound* self, float pitch) {
  Sound_EnsureState(self);
  FMODCALL(FMOD_Channel_SetPitch(self->handle, pitch));
}

void Sound_SetPlayPos (Sound* self, float seconds) {
  /* NOTE : Currently this has only millisecond accuracy. */
  Sound_EnsureState(self);
  Assert(seconds >= 0.0f);
  unsigned int ms = (unsigned int) Round(seconds * 1000.0f);
  FMODCALL(FMOD_Channel_SetPosition(self->handle, ms, FMOD_TIMEUNIT_MS));
}

void Sound_SetVolume (Sound* self, float volume) {
  Sound_EnsureState(self);
  FMODCALL(FMOD_Channel_SetVolume(self->handle, volume));
}

Sound* Sound_LoadPlay (cstr name, bool isLooped, bool is3D) {
  Sound* self = Sound_Load(name, isLooped, is3D);
  Sound_Play(self);
  return self;
}

Sound* Sound_LoadPlayAttached (cstr name, bool isLooped, bool is3D, Vec3f const* pos, Vec3f const* vel) {
  Sound* self = Sound_Load(name, isLooped, is3D);
  Sound_Attach3DPos(self, pos, vel);
  Sound_Play(self);
  return self;
}

void Sound_LoadPlayFree (cstr name, bool isLooped, bool is3D) {
  Sound* self = Sound_Load(name, isLooped, is3D);
  Sound_SetFreeOnFinish(self, true);
  Sound_Play(self);
}

void Sound_LoadPlayFreeAttached (cstr name, bool isLooped, bool is3D, Vec3f const* pos, Vec3f const* vel) {
  Sound* self = Sound_Load(name, isLooped, is3D);
  Sound_Attach3DPos(self, pos, vel);
  Sound_SetFreeOnFinish(self, true);
  Sound_Play(self);
}

Sound* Sound_ClonePlay (Sound* self) {
  Sound* clone = Sound_Clone(self);
  Sound_Play(clone);
  return clone;
}

Sound* Sound_ClonePlayAttached (Sound* self, Vec3f const* pos, Vec3f const* vel) {
  Sound* clone = Sound_Clone(self);
  Sound_Attach3DPos(clone, pos, vel);
  Sound_Play(clone);
  return clone;
}

void Sound_ClonePlayFree (Sound* self) {
  Sound* clone = Sound_Clone(self);
  Sound_SetFreeOnFinish(clone, true);
  Sound_Play(clone);
}

void Sound_ClonePlayFreeAttached (Sound* self, Vec3f const* pos, Vec3f const* vel) {
  Sound* clone = Sound_Clone(self);
  Sound_Attach3DPos(clone, pos, vel);
  Sound_SetFreeOnFinish(clone, true);
  Sound_Play(clone);
}

void Sound_Update (Sound* self) {
  if (self->state == SoundState_Loading) return;

  if (Sound_Get3D(self))
    Sound_Set3DPos(self, self->autoPos, self->autoVel);
}

bool Sound_IsFreed (Sound* self) {
  return self->state == SoundState_Freed;
}

/* NOTE : We start the sound instance in the paused state so that we can change
 *        position, pitch, etc. via the Sound API *before* samples start getting
 *        mixed. */

/* NOTE : By default, 3D channels are set to the *current position* of the
 *        listener! That's confusing and almost never what we want, so we reset
 *        the position immediately for consistency. */

/* NOTE : Generally self->state is updated through callbacks triggered by
 *        FMOD_System_Update and thus is slightly stale at all times. This means
 *        a sound could have finished earlier in the frame and we won't know
 *        until the beginning of the next frame when Audio_Update is called. */

/* OPTIMIZE : Sometimes we do redundant work:
 *            - When loading a sound synchronously or pausing a currently
 *            loading sound we call FMOD_Channel_SetPaused on an already paused
 *            sound.
 *            - When freeing a loading sound we block to finish the load just to
 *            free the sound. */

#include "Audio.h"
#include "File.h"
#include "FMODError.h"
#include "PhxMemory.h"
#include "Resource.h"
#include "Sound.h"
#include "SoundDesc.h"
#include "SoundDef.h"
#include "PhxString.h"
#include "fmod/fmod.h"

void SoundDesc_FinishLoad (SoundDesc* self, cstr func) {
  bool warned = false;
  FMOD_OPENSTATE openState;
  while (true) {
    FMODCALL(FMOD_Sound_GetOpenState(self->handle, &openState, 0, 0, 0));

    if (openState == FMOD_OPENSTATE_ERROR)
      Fatal("%s: Background file load has failed.\n  Path: %s", func, self->path);

    if (openState == FMOD_OPENSTATE_READY || openState == FMOD_OPENSTATE_PLAYING)
      break;

    if (!warned) {
      warned = true;
      Warn("%s: Background file load hasn't finished. Blocking the main thread.\n  Path: %s", func, self->path);
    }
  }
}
#define SoundDesc_FinishLoad(...) SoundDesc_FinishLoad(__VA_ARGS__, __func__)

SoundDesc* SoundDesc_Load (cstr name, bool immediate, bool isLooped, bool is3D) {
  cstr mapKey = StrAdd(isLooped ? "LOOPED:" : "UNLOOPED:", name);
  SoundDesc* self = Audio_AllocSoundDesc(mapKey);
  StrFree(mapKey);

  if (!self->name) {
    cstr path = Resource_GetPath(ResourceType_Sound, name);
    FMOD_MODE mode = 0;
    mode |= FMOD_CREATESAMPLE;
    mode |= FMOD_IGNORETAGS;
    mode |= FMOD_ACCURATETIME;
    mode |= isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= is3D ? (FMOD_3D | FMOD_3D_WORLDRELATIVE) : FMOD_2D;
    if (!immediate)
      mode |= FMOD_NONBLOCKING;

    FMODCALL(FMOD_System_CreateSound((FMOD_SYSTEM*) Audio_GetHandle(), path, mode, 0, &self->handle));
    FMODCALL(FMOD_Sound_SetUserData(self->handle, self));

    self->name = StrDup(name);
    self->path = StrDup(path);
    RefCounted_Init(self);
  } else {
    RefCounted_Acquire(self);

    if (immediate)
      SoundDesc_FinishLoad(self);
  }

  return self;
}

void SoundDesc_Acquire (SoundDesc* self) {
  RefCounted_Acquire(self);
}

void SoundDesc_Free (SoundDesc* self) {
  RefCounted_Free(self) {
    cstr name = self->name;
    cstr path = self->path;
    FMODCALL(FMOD_Sound_Release(self->handle));
    Audio_DeallocSoundDesc(self);
    StrFree(name);
    StrFree(path);
    /* TODO : Remove when StrMap_Remove is implemented */
    MemZero(self, sizeof(SoundDesc));
  }
}

float SoundDesc_GetDuration (SoundDesc* self) {
  SoundDesc_FinishLoad(self);

  uint32 duration;
  FMODCALL(FMOD_Sound_GetLength(self->handle, &duration, FMOD_TIMEUNIT_MS));
  return (float) duration / 1000.0f;
}

cstr SoundDesc_GetName (SoundDesc* self) {
  return self->name;
}

cstr SoundDesc_GetPath (SoundDesc* self) {
  return self->path;
}

void SoundDesc_ToFile (SoundDesc* self, cstr name) {
  /* TODO : Finish this.
   *        There's some sort of signed/unsigned issue with the current
   *        implementation. 8-bit PCM is unsigned according to the spec.
   *        However, inspecting thybidding.wav in a hex editor sure looks like
   *        signed data to me. The data read from FMOD is 'correct' but appears
   *        to be unsigned (e.g. offset by a constant 0x80 / 0d128).
   *
   *        I'm extremely confused hy this as the FMOD data seems to be correct
   *        while the actual file appears to be incorrect, yet VLC and Audacity
   *        both play the original file correctly and the file output here
   *        sounds like ass. Further, exporting the original wav to a new file
   *        looks like the original file, further supporting that it is correct
   *        and well formed.
   *
   *        Possible solutions:
   *        1) Post a question on the FMOD site to gather more information.
   *        2) Use Sound::readData instead (probably going to get the same result)
   *        3) Create a second System and use System::setOutput to set
   *           FMOD_OUTPUTTYPE_WAVWRITER_NRT
   *
   *        I've already spent too much time on this so I'm tabling it until
   *        more of the audio API is fleshed out (namely, FMOD Studio is
   *        integrated, Rigidbody updates are processed, and HRTF solutions are
   *        explored).
   */
  SoundDesc_FinishLoad(self);

  uint32 length;
  int32 channels;
  int32 bitsPerSample;
  FMODCALL(FMOD_Sound_GetLength(self->handle, &length, FMOD_TIMEUNIT_RAWBYTES));
  FMODCALL(FMOD_Sound_GetFormat(self->handle, 0, 0, &channels, &bitsPerSample));
  int32 bytesPerSample = bitsPerSample / 8;

  float sampleRate;
  FMOD_Sound_GetDefaults(self->handle, &sampleRate, 0);

  void* ptr1; uint32 len1;
  void* ptr2; uint32 len2;
  FMODCALL(FMOD_Sound_Lock(self->handle, 0, length, &ptr1, &ptr2, &len1, &len2));
  Assert(ptr2 == 0 && len2 == 0);
  Assert(len1 == length);

  /* Write the file */ {
    File* file = File_Create(name);
    if (!file)
      Fatal("SoundDesc_ToFile: Failed to create file.\nPath: %s", name);

    File_Write   (file, "RIFF", 4                                       ); // Chunk ID
    File_WriteI32(file, 36 + length                                     ); // Chunk Size
    File_Write   (file, "WAVE", 4                                       ); // Wave ID
    File_Write   (file, "fmt ", 4                                       ); // Chunk ID
    File_WriteI32(file, 16                                              ); // Chunk Size
    File_WriteI16(file, 1                                               ); // Format Code (PCM)
    File_WriteI16(file, (int16) channels                                ); // Channels
    File_WriteI32(file, (int32) sampleRate                              ); // Sample Rate
    File_WriteI32(file, (int32) (bytesPerSample * channels * sampleRate)); // Data Rate
    File_WriteI16(file, (int16) (bytesPerSample * channels)             ); // Frame Size
    File_WriteI16(file, (int16) (bitsPerSample)                         ); // Bits Per Sample
    File_Write   (file, "data", 4                                       );
    File_WriteI32(file, length                                          );
    File_Write   (file, ptr1, length                                    );
    File_Close   (file);
  }

  FMODCALL(FMOD_Sound_Unlock(self->handle, ptr1, ptr2, len1, len2));
}

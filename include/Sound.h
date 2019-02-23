#ifndef PHX_Sound
#define PHX_Sound

#include "Common.h"

/* --- Sound -------------------------------------------------------------------
 *
 *   A simple API for playing sounds.
 *
 *   Any operations on an asynchronously loaded sound will block until loading
 *   has finished if necessary.
 *
 *   Sound assets will automatically be unloaded once all sounds are freed.
 *
 *   This type is REFERENCE-COUNTED. See ../doc/RefCounted.txt for details.
 *
 * -------------------------------------------------------------------------- */

PHX_API void    Sound_Acquire                (Sound*);
PHX_API void    Sound_Free                   (Sound*);

PHX_API Sound*  Sound_Load                   (cstr name, bool isLooped, bool is3D);
PHX_API Sound*  Sound_LoadAsync              (cstr name, bool isLooped, bool is3D);
PHX_API Sound*  Sound_Clone                  (Sound*);
PHX_API void    Sound_ToFile                 (Sound*, cstr);

PHX_API void    Sound_Pause                  (Sound*);
PHX_API void    Sound_Play                   (Sound*);
PHX_API void    Sound_Rewind                 (Sound*);

PHX_API bool    Sound_Get3D                  (Sound*);
PHX_API float   Sound_GetDuration            (Sound*);
PHX_API bool    Sound_GetLooped              (Sound*);
PHX_API cstr    Sound_GetName                (Sound*);
PHX_API cstr    Sound_GetPath                (Sound*);
PHX_API bool    Sound_IsFinished             (Sound*);
PHX_API bool    Sound_IsPlaying              (Sound*);

PHX_API void    Sound_Attach3DPos            (Sound*, Vec3f const* pos, Vec3f const* vel);
PHX_API void    Sound_Set3DLevel             (Sound*, float);
PHX_API void    Sound_Set3DPos               (Sound*, Vec3f const* pos, Vec3f const* vel);
PHX_API void    Sound_SetFreeOnFinish        (Sound*, bool);
PHX_API void    Sound_SetPan                 (Sound*, float);
PHX_API void    Sound_SetPitch               (Sound*, float);
PHX_API void    Sound_SetPlayPos             (Sound*, float);
PHX_API void    Sound_SetVolume              (Sound*, float);

/* --- Convenience API ------------------------------------------------------ */

PHX_API Sound*  Sound_LoadPlay               (cstr name, bool isLooped, bool is3D);
PHX_API Sound*  Sound_LoadPlayAttached       (cstr name, bool isLooped, bool is3D, Vec3f const* pos, Vec3f const* vel);
PHX_API void    Sound_LoadPlayFree           (cstr name, bool isLooped, bool is3D);
PHX_API void    Sound_LoadPlayFreeAttached   (cstr name, bool isLooped, bool is3D, Vec3f const* pos, Vec3f const* vel);
PHX_API Sound*  Sound_ClonePlay              (Sound*);
PHX_API Sound*  Sound_ClonePlayAttached      (Sound*, Vec3f const* pos, Vec3f const* vel);
PHX_API void    Sound_ClonePlayFree          (Sound*);
PHX_API void    Sound_ClonePlayFreeAttached  (Sound*, Vec3f const* pos, Vec3f const* vel);

/* --- Private API ---------------------------------------------------------- */

PRIVATE void    Sound_Update                 (Sound*);
PRIVATE bool    Sound_IsFreed                (Sound*);

#endif

/* TODO : Will the sound re-load if reused? */

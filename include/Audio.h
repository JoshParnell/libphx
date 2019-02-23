#ifndef PHX_Audio
#define PHX_Audio

#include "Common.h"

PHX_API void        Audio_Init               ();
PHX_API void        Audio_Free               ();

PHX_API void        Audio_AttachListenerPos  (Vec3f const* pos, Vec3f const* vel, Vec3f const* fwd, Vec3f const* up);
PHX_API void        Audio_Set3DSettings      (float doppler, float scale, float rolloff);
PHX_API void        Audio_SetListenerPos     (Vec3f const* pos, Vec3f const* vel, Vec3f const* fwd, Vec3f const* up);
PHX_API void        Audio_Update             ();

/* --- Debug API ------------------------------------------------------------ */

PHX_API int32       Audio_GetLoadedCount     ();
PHX_API int32       Audio_GetPlayingCount    ();
PHX_API int32       Audio_GetTotalCount      ();

/* --- Private API ---------------------------------------------------------- */

PRIVATE void*       Audio_GetHandle          ();
PRIVATE SoundDesc*  Audio_AllocSoundDesc     (cstr name);
PRIVATE void        Audio_DeallocSoundDesc   (SoundDesc*);
PRIVATE Sound*      Audio_AllocSound         ();
PRIVATE void        Audio_DeallocSound       (Sound*);
PRIVATE void        Audio_SoundStateChanged  (Sound*);

#endif

/* NOTE : Primary */
/* TODO : Implement the FMOD Studio API and test with an FMOD project */
/* TODO : Investigate Oculus' HRTF */

/* NOTE : Secondary */
/* TODO : What happens when there is no audio device or the audio device is disconnected? */
/* TODO : Finish Sound_ToFile */
/* TODO : Where is CoInitialize being called? I don't see a warning from FMOD */
/* TODO : Respect the default device if it changes at runtime. This can't be done
 *        with FMOD alone. We'll need to detect default device changes some other way.
 *        http://www.fmod.org/questions/question/detecting-playback-device-change-in-windows/ */

/* NOTE : If we ever decide to use streams or internet sounds all sound APIs
 *        must be carefully updated to support it. In those cases channels have
 *        additional open states that we don't currently handle. */

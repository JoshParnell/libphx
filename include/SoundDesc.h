#ifndef PHX_SoundDesc
#define PHX_SoundDesc

#include "Common.h"

/* --- Private API ---------------------------------------------------------- */

PRIVATE SoundDesc*  SoundDesc_Load         (cstr name, bool immediate, bool isLooped, bool is3D);
PRIVATE void        SoundDesc_FinishLoad   (SoundDesc*, cstr func);
PRIVATE void        SoundDesc_Acquire      (SoundDesc*);
PRIVATE void        SoundDesc_Free         (SoundDesc*);

PRIVATE float       SoundDesc_GetDuration  (SoundDesc*);
PRIVATE cstr        SoundDesc_GetName      (SoundDesc*);
PRIVATE cstr        SoundDesc_GetPath      (SoundDesc*);
PRIVATE void        SoundDesc_ToFile       (SoundDesc*, cstr);

#endif

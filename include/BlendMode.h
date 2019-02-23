#ifndef PHX_BlendMode
#define PHX_BlendMode

#include "Common.h"

const BlendMode BlendMode_Additive     = 0;
const BlendMode BlendMode_Alpha        = 1;
const BlendMode BlendMode_Disabled     = 2;
const BlendMode BlendMode_PreMultAlpha = 3;

PHX_API void  BlendMode_Pop               ();
PHX_API void  BlendMode_Push              (BlendMode);
PHX_API void  BlendMode_PushAdditive      ();
PHX_API void  BlendMode_PushAlpha         ();
PHX_API void  BlendMode_PushDisabled      ();
PHX_API void  BlendMode_PushPreMultAlpha  ();

#endif

#ifndef PHX_DepthTesh
#define PHX_DepthTesh

#include "Common.h"

PHX_API void  DepthTest_Pop           ();
PHX_API void  DepthTest_Push          (bool);
PHX_API void  DepthTest_PushDisabled  ();
PHX_API void  DepthTest_PushEnabled   ();

#endif

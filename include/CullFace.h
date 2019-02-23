#ifndef PHX_CullFace
#define PHX_CullFace

#include "Common.h"

const CullFace CullFace_None  = 0;
const CullFace CullFace_Back  = 1;
const CullFace CullFace_Front = 2;

PHX_API void  CullFace_Pop        ();
PHX_API void  CullFace_Push       (CullFace);
PHX_API void  CullFace_PushNone   ();
PHX_API void  CullFace_PushBack   ();
PHX_API void  CullFace_PushFront  ();

#endif

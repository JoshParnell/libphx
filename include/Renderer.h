#ifndef PHX_Renderer
#define PHX_Renderer

#include "Common.h"

PHX_API void Renderer_Init ();
PHX_API void Renderer_Free ();

PHX_API void Renderer_SetDownsampleFactor (int);
PHX_API void Renderer_SetShader           (Shader*);
PHX_API void Renderer_SetTransform        (Matrix* toWorld, Matrix* toLocal);

#endif

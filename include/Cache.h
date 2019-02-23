#ifndef PHX_Cache
#define PHX_Cache

#include "Common.h"

void    Cache_Clear     ();
Shader* Cache_GetShader (cstr vs, cstr fs);

PRIVATE void Cache_Init ();

#endif

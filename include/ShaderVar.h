#ifndef PHX_ShaderVar
#define PHX_ShaderVar

#include "Common.h"

PHX_API void* ShaderVar_Get         (cstr, ShaderVarType);
PHX_API void  ShaderVar_PushFloat   (cstr, float);
PHX_API void  ShaderVar_PushFloat2  (cstr, float, float);
PHX_API void  ShaderVar_PushFloat3  (cstr, float, float, float);
PHX_API void  ShaderVar_PushFloat4  (cstr, float, float, float, float);
PHX_API void  ShaderVar_PushInt     (cstr, int);
PHX_API void  ShaderVar_PushMatrix  (cstr, Matrix*);
PHX_API void  ShaderVar_PushTex1D   (cstr, Tex1D*);
PHX_API void  ShaderVar_PushTex2D   (cstr, Tex2D*);
PHX_API void  ShaderVar_PushTex3D   (cstr, Tex3D*);
PHX_API void  ShaderVar_PushTexCube (cstr, TexCube*);
PHX_API void  ShaderVar_Pop         (cstr);

PRIVATE void ShaderVar_Init ();
PRIVATE void ShaderVar_Free ();

#endif

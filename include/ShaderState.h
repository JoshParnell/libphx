#ifndef PHX_ShaderState
#define PHX_ShaderState

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *   This type is REFERENCE-COUNTED. See ../doc/RefCounted.txt for details.
 *
 * -------------------------------------------------------------------------- */

PHX_API ShaderState*  ShaderState_Create          (Shader*);
PHX_API void          ShaderState_Acquire         (ShaderState*);
PHX_API void          ShaderState_Free            (ShaderState*);
PHX_API ShaderState*  ShaderState_FromShaderLoad  (cstr vertName, cstr fragName);

PHX_API void          ShaderState_SetFloat        (ShaderState*, cstr, float);
PHX_API void          ShaderState_SetFloat2       (ShaderState*, cstr, float, float);
PHX_API void          ShaderState_SetFloat3       (ShaderState*, cstr, float, float, float);
PHX_API void          ShaderState_SetFloat4       (ShaderState*, cstr, float, float, float, float);
PHX_API void          ShaderState_SetInt          (ShaderState*, cstr, int);
PHX_API void          ShaderState_SetMatrix       (ShaderState*, cstr, Matrix*);
PHX_API void          ShaderState_SetTex1D        (ShaderState*, cstr, Tex1D*);
PHX_API void          ShaderState_SetTex2D        (ShaderState*, cstr, Tex2D*);
PHX_API void          ShaderState_SetTex3D        (ShaderState*, cstr, Tex3D*);
PHX_API void          ShaderState_SetTexCube      (ShaderState*, cstr, TexCube*);

PHX_API void          ShaderState_Start           (ShaderState*);
PHX_API void          ShaderState_Stop            (ShaderState*);

#endif

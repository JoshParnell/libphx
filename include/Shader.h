#ifndef PHX_Shader
#define PHX_Shader

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *   This type is REFERENCE-COUNTED. See ../doc/RefCounted.txt for details.
 *
 * -------------------------------------------------------------------------- */

PHX_API Shader*       Shader_Create         (cstr vertCode, cstr fragCode);
PHX_API Shader*       Shader_Load           (cstr vertName, cstr fragName);
PHX_API void          Shader_Acquire        (Shader*);
PHX_API void          Shader_Free           (Shader*);
PHX_API ShaderState*  Shader_ToShaderState  (Shader*);

PHX_API void          Shader_Start          (Shader*);
PHX_API void          Shader_Stop           (Shader*);

PHX_API uint          Shader_GetHandle      (Shader*);
PHX_API int           Shader_GetVariable    (Shader*, cstr);
PHX_API bool          Shader_HasVariable    (Shader*, cstr);

PHX_API void          Shader_ClearCache     ();
PHX_API void          Shader_SetFloat       (cstr, float);
PHX_API void          Shader_SetFloat2      (cstr, float, float);
PHX_API void          Shader_SetFloat3      (cstr, float, float, float);
PHX_API void          Shader_SetFloat4      (cstr, float, float, float, float);
PHX_API void          Shader_SetInt         (cstr, int);
PHX_API void          Shader_SetMatrix      (cstr, Matrix*);
PHX_API void          Shader_SetMatrixT     (cstr, Matrix*);
PHX_API void          Shader_SetTex1D       (cstr, Tex1D*);
PHX_API void          Shader_SetTex2D       (cstr, Tex2D*);
PHX_API void          Shader_SetTex3D       (cstr, Tex3D*);
PHX_API void          Shader_SetTexCube     (cstr, TexCube*);

PHX_API void          Shader_ISetFloat      (int, float);
PHX_API void          Shader_ISetFloat2     (int, float, float);
PHX_API void          Shader_ISetFloat3     (int, float, float, float);
PHX_API void          Shader_ISetFloat4     (int, float, float, float, float);
PHX_API void          Shader_ISetInt        (int, int);
PHX_API void          Shader_ISetMatrix     (int, Matrix*);
PHX_API void          Shader_ISetMatrixT    (int, Matrix*);
PHX_API void          Shader_ISetTex1D      (int, Tex1D*);
PHX_API void          Shader_ISetTex2D      (int, Tex2D*);
PHX_API void          Shader_ISetTex3D      (int, Tex3D*);
PHX_API void          Shader_ISetTexCube    (int, TexCube*);
#endif

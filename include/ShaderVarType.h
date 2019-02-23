#ifndef PHX_ShaderVarType
#define PHX_ShaderVarType

#include "Common.h"

const ShaderVarType ShaderVarType_None    = 0x0;
const ShaderVarType ShaderVarType_BEGIN   = 0x1;
const ShaderVarType ShaderVarType_Float   = 0x1;
const ShaderVarType ShaderVarType_Float2  = 0x2;
const ShaderVarType ShaderVarType_Float3  = 0x3;
const ShaderVarType ShaderVarType_Float4  = 0x4;
const ShaderVarType ShaderVarType_Int     = 0x5;
const ShaderVarType ShaderVarType_Int2    = 0x6;
const ShaderVarType ShaderVarType_Int3    = 0x7;
const ShaderVarType ShaderVarType_Int4    = 0x8;
const ShaderVarType ShaderVarType_Matrix  = 0x9;
const ShaderVarType ShaderVarType_Tex1D   = 0xA;
const ShaderVarType ShaderVarType_Tex2D   = 0xB;
const ShaderVarType ShaderVarType_Tex3D   = 0xC;
const ShaderVarType ShaderVarType_TexCube = 0xD;
const ShaderVarType ShaderVarType_END     = 0xD;
const ShaderVarType ShaderVarType_SIZE    = 0xD;

PHX_API ShaderVarType  ShaderVarType_FromStr      (cstr);
PHX_API cstr           ShaderVarType_GetGLSLName  (ShaderVarType);
PHX_API cstr           ShaderVarType_GetName      (ShaderVarType);
PHX_API int            ShaderVarType_GetSize      (ShaderVarType);

#endif

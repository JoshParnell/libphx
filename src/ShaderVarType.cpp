#include "Matrix.h"
#include "ShaderVarType.h"
#include "PhxString.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

ShaderVarType ShaderVarType_FromStr (cstr s) {
  for (ShaderVarType i = ShaderVarType_BEGIN; i <= ShaderVarType_END; ++i)
    if (StrEqual(s, ShaderVarType_GetGLSLName(i)))
      return i;
  return ShaderVarType_None;
}

cstr ShaderVarType_GetGLSLName (ShaderVarType self) {
  switch (self) {
    case ShaderVarType_Float:   return "float";
    case ShaderVarType_Float2:  return "vec2";
    case ShaderVarType_Float3:  return "vec3";
    case ShaderVarType_Float4:  return "vec4";
    case ShaderVarType_Int:     return "int";
    case ShaderVarType_Int2:    return "ivec2";
    case ShaderVarType_Int3:    return "ivec3";
    case ShaderVarType_Int4:    return "ivec4";
    case ShaderVarType_Matrix:  return "mat4";
    case ShaderVarType_Tex1D:   return "sampler1D";
    case ShaderVarType_Tex2D:   return "sampler2D";
    case ShaderVarType_Tex3D:   return "sampler3D";
    case ShaderVarType_TexCube: return "samplerCube";
  }
  return 0;
}

cstr ShaderVarType_GetName (ShaderVarType self) {
  switch (self) {
    case ShaderVarType_Float:   return "float";
    case ShaderVarType_Float2:  return "float2";
    case ShaderVarType_Float3:  return "float3";
    case ShaderVarType_Float4:  return "float4";
    case ShaderVarType_Int:     return "int";
    case ShaderVarType_Int2:    return "int2";
    case ShaderVarType_Int3:    return "int3";
    case ShaderVarType_Int4:    return "int4";
    case ShaderVarType_Matrix:  return "Matrix";
    case ShaderVarType_Tex1D:   return "Tex1D";
    case ShaderVarType_Tex2D:   return "Tex2D";
    case ShaderVarType_Tex3D:   return "Tex3D";
    case ShaderVarType_TexCube: return "TexCube";
  }
  return 0;
}

int ShaderVarType_GetSize (ShaderVarType self) {
  switch (self) {
    case ShaderVarType_Float:   return sizeof(float);
    case ShaderVarType_Float2:  return sizeof(Vec2f);
    case ShaderVarType_Float3:  return sizeof(Vec3f);
    case ShaderVarType_Float4:  return sizeof(Vec4f);
    case ShaderVarType_Int:     return sizeof(int);
    case ShaderVarType_Int2:    return sizeof(Vec2i);
    case ShaderVarType_Int3:    return sizeof(Vec3i);
    case ShaderVarType_Int4:    return sizeof(Vec4i);
    case ShaderVarType_Matrix:  return sizeof(Matrix*);
    case ShaderVarType_Tex1D:   return sizeof(Tex1D*);
    case ShaderVarType_Tex2D:   return sizeof(Tex2D*);
    case ShaderVarType_Tex3D:   return sizeof(Tex3D*);
    case ShaderVarType_TexCube: return sizeof(TexCube*);
  }
  return 0;
}

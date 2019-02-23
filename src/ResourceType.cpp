#include "ResourceType.h"

const ResourceType ResourceType_Font    = 0x0;
const ResourceType ResourceType_Mesh    = 0x1;
const ResourceType ResourceType_Other   = 0x2;
const ResourceType ResourceType_Script  = 0x3;
const ResourceType ResourceType_Shader  = 0x4;
const ResourceType ResourceType_Sound   = 0x5;
const ResourceType ResourceType_Tex1D   = 0x6;
const ResourceType ResourceType_Tex2D   = 0x7;
const ResourceType ResourceType_Tex3D   = 0x8;
const ResourceType ResourceType_TexCube = 0x9;

cstr ResourceType_ToString (ResourceType self) {
  switch (self) {
    case ResourceType_Font:    return "Font";
    case ResourceType_Mesh:    return "Mesh";
    case ResourceType_Other:   return "Other";
    case ResourceType_Script:  return "Script";
    case ResourceType_Shader:  return "Shader";
    case ResourceType_Sound:   return "Sound";
    case ResourceType_Tex1D:   return "Tex1D";
    case ResourceType_Tex2D:   return "Tex2D";
    case ResourceType_Tex3D:   return "Tex3D";
    case ResourceType_TexCube: return "TexCube";
  }
  return 0;
}

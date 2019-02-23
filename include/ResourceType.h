#ifndef PHX_ResourceType
#define PHX_ResourceType

#include "Common.h"

PHX_API const ResourceType ResourceType_Font;
PHX_API const ResourceType ResourceType_Mesh;
PHX_API const ResourceType ResourceType_Other;
PHX_API const ResourceType ResourceType_Script;
PHX_API const ResourceType ResourceType_Shader;
PHX_API const ResourceType ResourceType_Sound;
PHX_API const ResourceType ResourceType_Tex1D;
PHX_API const ResourceType ResourceType_Tex2D;
PHX_API const ResourceType ResourceType_Tex3D;
PHX_API const ResourceType ResourceType_TexCube;
const ResourceType ResourceType_COUNT = 0xA;

PHX_API cstr  ResourceType_ToString  (ResourceType);

#endif

#ifndef PHX_TexFormat
#define PHX_TexFormat

#include "Common.h"

PHX_API const TexFormat TexFormat_R8;
PHX_API const TexFormat TexFormat_R16;
PHX_API const TexFormat TexFormat_R16F;
PHX_API const TexFormat TexFormat_R32F;
PHX_API const TexFormat TexFormat_RG8;
PHX_API const TexFormat TexFormat_RG16;
PHX_API const TexFormat TexFormat_RG16F;
PHX_API const TexFormat TexFormat_RG32F;
PHX_API const TexFormat TexFormat_RGB8;
PHX_API const TexFormat TexFormat_RGBA8;
PHX_API const TexFormat TexFormat_RGBA16;
PHX_API const TexFormat TexFormat_RGBA16F;
PHX_API const TexFormat TexFormat_RGBA32F;

PHX_API const TexFormat TexFormat_Depth16;
PHX_API const TexFormat TexFormat_Depth24;
PHX_API const TexFormat TexFormat_Depth32F;

PHX_API int   TexFormat_Components  (TexFormat);
PHX_API int   TexFormat_GetSize     (TexFormat);
PHX_API bool  TexFormat_IsColor     (TexFormat);
PHX_API bool  TexFormat_IsDepth     (TexFormat);
PHX_API bool  TexFormat_IsValid     (TexFormat);

#endif

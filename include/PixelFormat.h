#ifndef PHX_PixelFormat
#define PHX_PixelFormat

#include "Common.h"

PHX_API const PixelFormat PixelFormat_Red;
PHX_API const PixelFormat PixelFormat_RG;
PHX_API const PixelFormat PixelFormat_RGB;
PHX_API const PixelFormat PixelFormat_BGR;
PHX_API const PixelFormat PixelFormat_RGBA;
PHX_API const PixelFormat PixelFormat_BGRA;
PHX_API const PixelFormat PixelFormat_Depth_Component;

PHX_API int  PixelFormat_Components  (PixelFormat);

#endif

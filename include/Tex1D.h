#ifndef PHX_Tex1D
#define PHX_Tex1D

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *   This type is REFERENCE-COUNTED. See ../doc/RefCounted.txt for details.
 *
 * -------------------------------------------------------------------------- */

PHX_API Tex1D*     Tex1D_Create        (int size, TexFormat);
PHX_API void       Tex1D_Acquire       (Tex1D*);
PHX_API void       Tex1D_Free          (Tex1D*);

PHX_API void       Tex1D_Draw          (Tex1D*, float x, float y, float xs, float ys);
PHX_API void       Tex1D_GenMipmap     (Tex1D*);
PHX_API void       Tex1D_GetData       (Tex1D*, void*, PixelFormat, DataFormat);
PHX_API Bytes*     Tex1D_GetDataBytes  (Tex1D*, PixelFormat, DataFormat);
PHX_API TexFormat  Tex1D_GetFormat     (Tex1D*);
PHX_API uint       Tex1D_GetHandle     (Tex1D*);
PHX_API uint       Tex1D_GetSize       (Tex1D*);
PHX_API void       Tex1D_SetData       (Tex1D*, void const*, PixelFormat, DataFormat);
PHX_API void       Tex1D_SetDataBytes  (Tex1D*, Bytes*, PixelFormat, DataFormat);
PHX_API void       Tex1D_SetMagFilter  (Tex1D*, TexFilter);
PHX_API void       Tex1D_SetMinFilter  (Tex1D*, TexFilter);
PHX_API void       Tex1D_SetTexel      (Tex1D*, int x, float r, float g, float b, float a);
PHX_API void       Tex1D_SetWrapMode   (Tex1D*, TexWrapMode);

#endif

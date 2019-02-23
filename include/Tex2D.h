#ifndef PHX_Tex2D
#define PHX_Tex2D

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *   This type is REFERENCE-COUNTED. See ../doc/RefCounted.txt for details.
 *
 * -------------------------------------------------------------------------- */

PHX_API Tex2D*     Tex2D_Create         (int sx, int sy, TexFormat);
PHX_API Tex2D*     Tex2D_Load           (cstr name);
PHX_API Tex2D*     Tex2D_ScreenCapture  ();
PHX_API void       Tex2D_Acquire        (Tex2D*);
PHX_API void       Tex2D_Free           (Tex2D*);

PHX_API void       Tex2D_Pop            (Tex2D*);
PHX_API void       Tex2D_Push           (Tex2D*);
PHX_API void       Tex2D_PushLevel      (Tex2D*, int level);

PHX_API void       Tex2D_Clear          (Tex2D*, float r, float g, float b, float a);
PHX_API Tex2D*     Tex2D_Clone          (Tex2D*);
PHX_API void       Tex2D_Draw           (Tex2D*, float x, float y, float sx, float sy);
PHX_API void       Tex2D_DrawEx         (Tex2D*, float x0, float y0, float x1, float y1,
                                                 float u0, float v0, float u1, float v1);
PHX_API void       Tex2D_GenMipmap      (Tex2D*);
PHX_API void       Tex2D_GetData        (Tex2D*, void*, PixelFormat, DataFormat);
PHX_API Bytes*     Tex2D_GetDataBytes   (Tex2D*, PixelFormat, DataFormat);
PHX_API TexFormat  Tex2D_GetFormat      (Tex2D*);
PHX_API uint       Tex2D_GetHandle      (Tex2D*);
PHX_API void       Tex2D_GetSize        (Tex2D*, Vec2i* out);
PHX_API void       Tex2D_GetSizeLevel   (Tex2D*, Vec2i* out, int level);
PHX_API void       Tex2D_SetAnisotropy  (Tex2D*, float);
PHX_API void       Tex2D_SetData        (Tex2D*, void const*, PixelFormat, DataFormat);
PHX_API void       Tex2D_SetDataBytes   (Tex2D*, Bytes*, PixelFormat, DataFormat);
PHX_API void       Tex2D_SetMagFilter   (Tex2D*, TexFilter);
PHX_API void       Tex2D_SetMinFilter   (Tex2D*, TexFilter);
PHX_API void       Tex2D_SetMipRange    (Tex2D*, int minLevel, int maxLevel);
PHX_API void       Tex2D_SetTexel       (Tex2D*, int x, int y, float r, float g, float b, float a);
PHX_API void       Tex2D_SetWrapMode    (Tex2D*, TexWrapMode);

PHX_API void       Tex2D_Save           (Tex2D*, cstr path);

#endif

#ifndef PHX_Tex3D
#define PHX_Tex3D

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *   This type is REFERENCE-COUNTED. See ../doc/RefCounted.txt for details.
 *
 * -------------------------------------------------------------------------- */

PHX_API Tex3D*     Tex3D_Create        (int sx, int sy, int sz, TexFormat);
PHX_API void       Tex3D_Acquire       (Tex3D*);
PHX_API void       Tex3D_Free          (Tex3D*);

PHX_API void       Tex3D_Pop            (Tex3D*);
PHX_API void       Tex3D_Push           (Tex3D*, int layer);
PHX_API void       Tex3D_PushLevel      (Tex3D*, int layer, int level);

PHX_API void       Tex3D_Draw          (Tex3D* self, int layer, float x, float y, float xs, float ys);
PHX_API void       Tex3D_GenMipmap     (Tex3D*);
PHX_API void       Tex3D_GetData       (Tex3D*, void*, PixelFormat, DataFormat);
PHX_API Bytes*     Tex3D_GetDataBytes  (Tex3D*, PixelFormat, DataFormat);
PHX_API TexFormat  Tex3D_GetFormat     (Tex3D*);
PHX_API uint       Tex3D_GetHandle     (Tex3D*);
PHX_API void       Tex3D_GetSize       (Tex3D*, Vec3i* out);
PHX_API void       Tex3D_GetSizeLevel  (Tex3D*, Vec3i* out, int level);
PHX_API void       Tex3D_SetData       (Tex3D*, void const*, PixelFormat, DataFormat);
PHX_API void       Tex3D_SetDataBytes  (Tex3D*, Bytes*, PixelFormat, DataFormat);
PHX_API void       Tex3D_SetMagFilter  (Tex3D*, TexFilter);
PHX_API void       Tex3D_SetMinFilter  (Tex3D*, TexFilter);
PHX_API void       Tex3D_SetWrapMode   (Tex3D*, TexWrapMode);

#endif

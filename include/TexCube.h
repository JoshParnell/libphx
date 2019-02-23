#ifndef PHX_TexCube
#define PHX_TexCube

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *   This type is REFERENCE-COUNTED. See ../doc/RefCounted.txt for details.
 *
 * -------------------------------------------------------------------------- */

PHX_API TexCube*   TexCube_Create        (int size, TexFormat);
PHX_API TexCube*   TexCube_Load          (cstr path);
PHX_API void       TexCube_Acquire       (TexCube*);
PHX_API void       TexCube_Free          (TexCube*);

PHX_API void       TexCube_Clear         (TexCube*, float r, float g, float b, float a);
PHX_API void       TexCube_Generate      (TexCube*, ShaderState*);
PHX_API TexCube*   TexCube_GenIRMap      (TexCube*, int samples);
PHX_API void       TexCube_GenMipmap     (TexCube*);
PHX_API void       TexCube_GetData       (TexCube*, void*, CubeFace, int level, PixelFormat, DataFormat);
PHX_API Bytes*     TexCube_GetDataBytes  (TexCube*, CubeFace, int level, PixelFormat, DataFormat);
PHX_API TexFormat  TexCube_GetFormat     (TexCube*);
PHX_API uint       TexCube_GetHandle     (TexCube*);
PHX_API int        TexCube_GetSize       (TexCube*);
PHX_API void       TexCube_SetData       (TexCube*, void const*, CubeFace, int level, PixelFormat, DataFormat);
PHX_API void       TexCube_SetDataBytes  (TexCube*, Bytes*, CubeFace, int level, PixelFormat, DataFormat);
PHX_API void       TexCube_SetMagFilter  (TexCube*, TexFilter);
PHX_API void       TexCube_SetMinFilter  (TexCube*, TexFilter);

PHX_API void       TexCube_Save          (TexCube*, cstr path);
PHX_API void       TexCube_SaveLevel     (TexCube*, cstr path, int level);

#endif

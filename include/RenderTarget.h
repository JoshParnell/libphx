#ifndef PHX_RenderTarget
#define PHX_RenderTarget

#include "Common.h"

/* --- RenderTarget ------------------------------------------------------------
 *
 *   The RenderTarget_*Level variants bind a specific mip level of the given
 *   texture. Level 0 is the primary texture and the default for the non-level
 *   variants; level 1 is the first mip level, level log2(res) is the final
 *   (single-texel) mip level.
 *
 *   The RenderTarget_PushTex* functions are convenience wrappers for binding
 *   a single texture as the sole framebuffer target. The Push/Pop and Bind
 *   functions must be used if a more advanced framebuffer configuration is
 *   needed (e.g. multiple output surfaces, depth attachments, etc.)
 *
 * -------------------------------------------------------------------------- */

PHX_API void  RenderTarget_Push              (int sx, int sy);
PHX_API void  RenderTarget_Pop               ();

PHX_API void  RenderTarget_BindTex2D         (Tex2D*);
PHX_API void  RenderTarget_BindTex2DLevel    (Tex2D*, int level);
PHX_API void  RenderTarget_BindTex3D         (Tex3D*, int layer);
PHX_API void  RenderTarget_BindTex3DLevel    (Tex3D*, int layer, int level);
PHX_API void  RenderTarget_BindTexCube       (TexCube*, CubeFace);
PHX_API void  RenderTarget_BindTexCubeLevel  (TexCube*, CubeFace, int level);

PHX_API void  RenderTarget_PushTex2D         (Tex2D*);
PHX_API void  RenderTarget_PushTex2DLevel    (Tex2D*, int level);
PHX_API void  RenderTarget_PushTex3D         (Tex3D*, int layer);
PHX_API void  RenderTarget_PushTex3DLevel    (Tex3D*, int layer, int level);

#endif

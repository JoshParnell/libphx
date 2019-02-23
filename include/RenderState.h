#ifndef PHX_RenderState
#define PHX_RenderState

#include "Common.h"

/* WARNING : Per the OpenGL specs, DepthWritable is irrelevant if DepthTest is
 *           off. Specifically, turning DepthTest off and DepthWritable on does
 *           NOT function as a 'depth overwrite' mode as might be expected! */

PHX_API void  RenderState_PushAllDefaults    ();
PHX_API void  RenderState_PushBlendMode      (BlendMode);
PHX_API void  RenderState_PushCullFace       (CullFace);
PHX_API void  RenderState_PushDepthTest      (bool);
PHX_API void  RenderState_PushDepthWritable  (bool);
PHX_API void  RenderState_PushWireframe      (bool);

PHX_API void  RenderState_PopAll             ();
PHX_API void  RenderState_PopBlendMode       ();
PHX_API void  RenderState_PopCullFace        ();
PHX_API void  RenderState_PopDepthTest       ();
PHX_API void  RenderState_PopDepthWritable   ();
PHX_API void  RenderState_PopWireframe       ();

#endif

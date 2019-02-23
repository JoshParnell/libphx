#ifndef PHX_Octree
#define PHX_Octree

#include "Common.h"
#include "Box3.h"

PHX_API Octree*  Octree_Create        (Box3f bound);
PHX_API Octree*  Octree_FromMesh      (Mesh*);
PHX_API void     Octree_Free          (Octree*);

PHX_API void     Octree_Add           (Octree*, Box3f bound, uint32 id);
PHX_API void     Octree_Draw          (Octree*);
PHX_API double   Octree_GetAvgLoad    (Octree*);
PHX_API int      Octree_GetMaxLoad    (Octree*);
PHX_API int      Octree_GetMemory     (Octree*);
PHX_API bool     Octree_IntersectRay  (Octree*, Matrix*, Vec3f const* ro, Vec3f const* rd);

#endif

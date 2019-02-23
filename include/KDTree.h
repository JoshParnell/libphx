#ifndef PHX_KDTree
#define PHX_KDTree

#include "Common.h"
#include "Box3.h"

PHX_API KDTree*  KDTree_FromMesh      (Mesh*);
PHX_API void     KDTree_Free          (KDTree*);

PHX_API void     KDTree_Draw          (KDTree*, int maxDepth);
PHX_API int      KDTree_GetMemory     (KDTree*);
PHX_API bool     KDTree_IntersectRay  (KDTree*, Matrix*, Vec3f const* ro, Vec3f const* rd);

#endif

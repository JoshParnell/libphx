#ifndef PHX_BoxTree
#define PHX_BoxTree

#include "Common.h"
#include "Box3.h"

PHX_API BoxTree*  BoxTree_Create        ();
PHX_API BoxTree*  BoxTree_FromMesh      (Mesh*);
PHX_API void      BoxTree_Free          (BoxTree*);

PHX_API void      BoxTree_Add           (BoxTree*, Box3f bound, void* data);
PHX_API void      BoxTree_Draw          (BoxTree*, int maxDepth);
PHX_API int       BoxTree_GetMemory     (BoxTree*);
PHX_API bool      BoxTree_IntersectRay  (BoxTree*, Matrix*, Vec3f const* ro, Vec3f const* rd);

#endif

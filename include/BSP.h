#ifndef PHX_BSP
#define PHX_BSP

#include "ArrayList.h"
#include "Intersect.h"
#include "Mesh.h"
#include "Vec3.h"

PHX_API BSP*  BSP_Create                (Mesh*);
PHX_API void  BSP_Free                  (BSP*);

PHX_API bool  BSP_IntersectRay          (BSP*, Ray const*, float* tHit);
PHX_API bool  BSP_IntersectLineSegment  (BSP*, LineSegment const*, Vec3f* pHit);
PHX_API bool  BSP_IntersectSphere       (BSP*, Sphere const*, Vec3f* pHit);

/* --- Debug API ------------------------------------------------------------ */

PHX_API const BSPNodeRel BSPNodeRel_Parent;
PHX_API const BSPNodeRel BSPNodeRel_Back;
PHX_API const BSPNodeRel BSPNodeRel_Front;

struct BSPNodeRef {
  int32 index;
  uint8 triangleCount;
};

struct TriangleTest {
  Triangle* triangle;
  bool      hit;
};

struct IntersectSphereProfiling {
  int32                   nodes;
  int32                   leaves;
  int32                   triangles;
  ArrayList(TriangleTest, triangleTests);
};

PHX_API BSPNodeRef  BSPDebug_GetNode                      (BSP*, BSPNodeRef, BSPNodeRel);
PHX_API void        BSPDebug_DrawNode                     (BSP*, BSPNodeRef);
PHX_API void        BSPDebug_DrawNodeSplit                (BSP*, BSPNodeRef);
PHX_API void        BSPDebug_DrawLineSegment              (BSP*, LineSegment*);
PHX_API void        BSPDebug_DrawSphere                   (BSP*, Sphere*);
PHX_API void        BSPDebug_PrintRayProfilingData        (BSP*, double totalTime);
PHX_API void        BSPDebug_PrintSphereProfilingData     (BSP*, double totalTime);

PHX_API bool        BSPDebug_GetIntersectSphereTriangles  (BSP*, Sphere*, IntersectSphereProfiling* sphereProf);
PHX_API BSPNodeRef  BSPDebug_GetLeaf                      (BSP*, int32 leafIndex);

#endif

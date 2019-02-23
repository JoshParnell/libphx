#ifndef PHX_HashGrid
#define PHX_HashGrid

#include "Common.h"

/* --- HashGrid ----------------------------------------------------------------
 *
 *   A flat, uniform-grid-backed spatial partitioning structure. Much like a
 *   HashMap, the HashGrid has no hierarchy, and instead uses hashing to
 *   achieve (amortized) O(1) lookups. Since chunks of space are mapped via a
 *   hash function to their corresponding bucket, HashGrids have the nice
 *   property of easily supporting unbounded, highly-sparse spaces without
 *   requiring any cleverness.
 *
 *   The primary drawback of the HashGrid is in the lack of hierarchy. Large
 *   objects will take up asymptotically more grid cells. Large, moving objects
 *   are pathological for grids, since a cubic amount of space must be cleared
 *   and then re-inserted when an object moves. Hence, this structure is most
 *   appropriate for uniformly or nearly-uniformly-sized objects, and performs
 *   very well with large numbers of such objects.
 *
 * -------------------------------------------------------------------------- */

PHX_API HashGrid*      HashGrid_Create      (float cellSize, uint32 cellCount);
PHX_API void           HashGrid_Free        (HashGrid*);

PHX_API HashGridElem*  HashGrid_Add         (HashGrid*, void* object, Box3f const*);
PHX_API void           HashGrid_Clear       (HashGrid*);
PHX_API void           HashGrid_Remove      (HashGrid*, HashGridElem*);
PHX_API void           HashGrid_Update      (HashGrid*, HashGridElem*, Box3f const*);

PHX_API void**         HashGrid_GetResults  (HashGrid*);
PHX_API int            HashGrid_QueryBox    (HashGrid*, Box3f const*);
PHX_API int            HashGrid_QueryPoint  (HashGrid*, Vec3f const*);

#endif

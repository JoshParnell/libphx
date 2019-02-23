#ifndef PHX_HashMap
#define PHX_HashMap

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *   HashMap provides (amortized) O(1) value lookups for arbitrary key types.
 *   Since key length must be provided at creation time, any given HashMap is
 *   essentially parametrized by a single key type (HashMap<T>).
 *
 *   WARNING : This HashMap API is designed for speed rather than guaranteed
 *             accuracy. It does not store the key, but rather a 64-bit hash
 *             thereof. This trade-off allows for extreme speed and compactness
 *             compared to non-lossy maps. However, it also introduces a non-
 *             negligible probability of collision events causing the map to
 *             incorrectly return a false positive or to replace an element
 *             with another that has a different (hash-colliding) key. One can
 *             expect such an error to occur roughly once per 2^32 mappings.
 *
 *   NOTE : Remove is not yet implemented. It is very difficult to implement
 *          efficiently :) Besides, this map is so fast that you probably
 *          should just create and tear a new one down every time you need it.
 *
 * ---------------------------------------------------------------------------*/

typedef void (*ValueForeach)(void* value, void* userData);

PHX_API HashMap*  HashMap_Create   (uint32 keySize, uint32 capacity);
PHX_API void      HashMap_Free     (HashMap*);

PHX_API void      HashMap_Foreach  (HashMap*, ValueForeach, void* userData);
PHX_API void*     HashMap_Get      (HashMap*, void const* key);
PHX_API void*     HashMap_GetRaw   (HashMap*, uint64 keyHash);
PHX_API void      HashMap_Resize   (HashMap*, uint32 capacity);
PHX_API void      HashMap_Set      (HashMap*, void const* key, void* value);
PHX_API void      HashMap_SetRaw   (HashMap*, uint64 keyHash, void* value);

#endif

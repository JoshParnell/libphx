#ifndef PHX_MemPool
#define PHX_MemPool

#include "Common.h"

PHX_API MemPool*  MemPool_Create      (uint32 cellSize, uint32 blockSize);
PHX_API MemPool*  MemPool_CreateAuto  (uint32 elemSize);
PHX_API void      MemPool_Free        (MemPool*);

PHX_API void*     MemPool_Alloc       (MemPool*);
PHX_API void      MemPool_Clear       (MemPool*);
PHX_API void      MemPool_Dealloc     (MemPool*, void*);

PHX_API uint32    MemPool_GetCapacity (MemPool*);
PHX_API uint32    MemPool_GetSize     (MemPool*);

#endif

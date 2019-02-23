#ifndef PHX_MemStack
#define PHX_MemStack

#include "Common.h"

/* --- MemStack ----------------------------------------------------------------
 *
 *    Provides a fixed-size, stack-based allocator, allowing for nearly-instant
 *    alloc, dealloc, and clear. This is the fastest allocator, but has the
 *    strictest allocation pattern requirements.
 *
 *    Allocations MUST be properly paired, otherwise memory corruption will
 *    ensue. The last allocation must always be the first free, etc.
 *
 *    Note that freeing is not necessary if using clear, making this a good
 *    choice for intra-frame allocations, or any allocations with a bounded
 *    lifetime.
 *
 *    For a dynamically-sized version, use MemStackDynamic, which leverages
 *    multiple MemStacks to provide the same interface without requiring a
 *    pre-determined capacity.
 *
 * -------------------------------------------------------------------------- */

PHX_API MemStack* MemStack_Create  (uint32 capacity);
PHX_API void      MemStack_Free    (MemStack*);

PHX_API void*     MemStack_Alloc   (MemStack*, uint32 size);
PHX_API void      MemStack_Clear   (MemStack*);
PHX_API void      MemStack_Dealloc (MemStack*, uint32 size);

PHX_API bool      MemStack_CanAlloc     (MemStack*, uint32 size);
PHX_API uint32    MemStack_GetSize      (MemStack*);
PHX_API uint32    MemStack_GetCapacity  (MemStack*);
PHX_API uint32    MemStack_GetRemaining (MemStack*);

#endif

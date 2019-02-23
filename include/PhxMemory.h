#ifndef PHX_Memory
#define PHX_Memory

#include "Common.h"

#include <stdlib.h>
#include <string.h>

inline void*  MemAlloc      (size_t size);
inline void*  MemAllocZero  (size_t size);
inline void   MemCpy        (void* dst, void const* src, size_t size);
inline void   MemMove       (void* dst, void const* src, size_t size);
inline void   MemFree       (void const* ptr);
inline void*  MemRealloc    (void* ptr, size_t newSize);
inline void   MemSet        (void* dst, int value, size_t size);
inline void   MemZero       (void* dst, size_t size);

#define MemNew(x)             ((x*)MemAlloc(sizeof(x)))
#define MemNewZero(x)         ((x*)MemAllocZero(sizeof(x)))
#define MemNewArray(x, s)     ((x*)MemAlloc(sizeof(x) * (s)))
#define MemNewArrayZero(x, s) ((x*)MemAllocZero(sizeof(x) * (s)))

/* Exported versions for applications that need to ensure usage of the same
 * memory allocator as libphx. */
PHX_API void*  Memory_Alloc    (size_t);
PHX_API void*  Memory_Calloc   (size_t n, size_t size);
PHX_API void   Memory_Free     (void* ptr);
PHX_API void   Memory_MemCopy  (void* dst, void const* src, size_t size);
PHX_API void   Memory_MemMove  (void* dst, void const* src, size_t size);
PHX_API void*  Memory_Realloc  (void* ptr, size_t newSize);

/* -------------------------------------------------------------------------- */

inline void* MemAlloc (size_t size) {
  return malloc(size);
}

inline void* MemAllocZero (size_t size) {
  return calloc(1, size);
}

inline void MemCpy (void* dst, void const* src, size_t size) {
  memcpy(dst, src, size);
}

inline void MemMove (void* dst, void const* src, size_t size) {
  memmove(dst, src, size);
}

inline void MemFree (void const* ptr) {
  free((void*)ptr);
}

inline void* MemRealloc (void* ptr, size_t newSize) {
  return realloc(ptr, newSize);
}

inline void MemSet (void* dst, int value, size_t size) {
  memset(dst, value, size);
}

inline void MemZero (void* dst, size_t size) {
  memset(dst, 0, size);
}

#endif

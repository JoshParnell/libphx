#include "PhxMemory.h"

void* Memory_Alloc (size_t size) {
  return malloc(size);
}

void* Memory_Calloc (size_t n, size_t size) {
  return calloc(n, size);
}

void Memory_Free (void* ptr) {
  free(ptr);
}

void Memory_MemCopy (void* dst, void const* src, size_t size) {
  memcpy(dst, src, size);
}

void Memory_MemMove (void* dst, void const* src, size_t size) {
  memmove(dst, src, size);
}

void* Memory_Realloc (void* ptr, size_t newSize) {
  return realloc(ptr, newSize);
}

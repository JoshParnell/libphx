#include "MemPool.h"
#include "PhxMemory.h"
#include "PhxMath.h"

#define PAGE_SIZE 0x1000

/* NOTE : At the moment, a great deal of the CTypes code relies on mempool
 *        cells being zero-initialized on allocation. */
#define ZERO_MEMORY 1

/* Zero cell memory upon being released back to the pool. Should not be used
 * in release builds due to performance impact, but very useful for debugging
 * use-after-free memory errors. */
#define ZERO_ON_FREE 0

struct MemPool {
  uint32 size;
  uint32 capacity;
  void*  freeList;
  uint32 cellSize;
  uint32 blockSize;
  uint16 blockCount;
  void** blocks;
};

static void MemPool_Grow (MemPool* self) {
  uint16 newBlockIndex = self->blockCount++;
  self->capacity += self->blockSize;

  /* Grow the list of pool blocks. */
  self->blocks = (void**)MemRealloc(self->blocks, self->blockCount * sizeof(void*));

  /* Allocate a new block and place at the back of the list. */
  void* newBlock = MemAlloc(self->cellSize * self->blockSize);
  self->blocks[newBlockIndex] = newBlock;

  /* Wire up the free list for this block. Note that we can assume the existing
   * free list is empty if the pool is requesting to grow, hence we overwrite
   * the existing list head. The block's initial freelist is wired sequentially
   * ( 0 -> 1 -> 2 ) for optimal cache locality. */
  void** prev = &self->freeList;
  char* pCurr = (char*)newBlock;
  for (uint32 i = 0; i < self->blockSize; ++i) {
    *prev = (void*)pCurr;
    prev = (void**)pCurr;
    pCurr += self->cellSize;
  }
  *prev = 0;
}

MemPool* MemPool_Create (uint32 cellSize, uint32 blockSize) {
  MemPool* self = MemNew(MemPool);
  self->size = 0;
  self->capacity = 0;
  self->freeList = 0;
  self->cellSize = cellSize;
  self->blockSize = blockSize;
  self->blockCount = 0;
  self->blocks = 0;
  return self;
}

MemPool* MemPool_CreateAuto (uint32 elemSize) {
  return MemPool_Create(elemSize, PAGE_SIZE / elemSize);
}

void MemPool_Free (MemPool* self) {
  for (uint16 i = 0; i < self->blockCount; ++i)
    MemFree(self->blocks[i]);
  MemFree(self->blocks);
}

void* MemPool_Alloc (MemPool* self) {
  IF_UNLIKELY (self->size == self->capacity)
    MemPool_Grow(self);

  /* Grab the freeList's head block and replace freeList with the value therein
   * (the internal freelist next pointer) */
  void* freeCell = self->freeList;
  self->freeList = *(void**)freeCell;
  self->size++;

#if ZERO_MEMORY
  MemZero(freeCell, self->cellSize);
#endif

  return freeCell;
}

void MemPool_Clear (MemPool* self) {
  self->size = 0;
  self->freeList = 0;

  /* We must take care to wire the freelist properly over block boundaries when
   * clearing. */
  void** prev = &self->freeList;
  for (uint32 i = 0; i < self->blockCount; ++i) {
    char* pCurr = (char*)self->blocks[i];
    for (uint32 j = 0; j < self->blockSize; ++j) {
      *prev = (void*)pCurr;
      prev = (void**)pCurr;
      pCurr += self->cellSize;
    }
  }
  *prev = 0;
}

void MemPool_Dealloc (MemPool* self, void* ptr) {
#if ZERO_ON_FREE
  MemZero(ptr, self->cellSize);
#endif
  *(void**)ptr = self->freeList;
  self->freeList = ptr;
  self->size--;
}

uint32 MemPool_GetCapacity (MemPool* self) {
  return self->capacity;
}

uint32 MemPool_GetSize (MemPool* self) {
  return self->size;
}

#include "MemStack.h"
#include "PhxMemory.h"

struct MemStack {
  uint32 size;
  uint32 capacity;
  void* data;
};

MemStack* MemStack_Create (uint32 capacity) {
  MemStack* self = MemNew(MemStack);
  self->size = 0;
  self->capacity = capacity;
  self->data = MemAlloc(capacity);
  return self;
};

void MemStack_Free (MemStack* self) {
  MemFree(self->data);
  MemFree(self);
}

void* MemStack_Alloc (MemStack* self, uint32 size) {
  if (self->size + size > self->capacity)
    Fatal("MemStack_Alloc: Allocation request exceeds remaining capacity");
  void* p = (char*)self->data + self->size;
  self->size += size;
  return p;
}

void MemStack_Clear (MemStack* self) {
  self->size = 0;
}

void MemStack_Dealloc (MemStack* self, uint32 size) {
  if (self->size < size)
    Fatal("MemStack_Dealloc: Attempt to dealloc more memory than is allocated");
  self->size -= size;
}

bool MemStack_CanAlloc (MemStack* self, uint32 size) {
  return self->size + size <= self->capacity;
}

uint32 MemStack_GetSize (MemStack* self) {
  return self->size;
}

uint32 MemStack_GetCapacity (MemStack* self) {
  return self->capacity;
}

uint32 MemStack_GetRemaining (MemStack* self) {
  return self->capacity - self->size;
}

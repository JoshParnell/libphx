#include "Hash.h"
#include "HashMap.h"
#include "PhxMemory.h"

#include <stdlib.h>
#include <string.h>

struct Node {
  uint64 hash;
  void*  value;
};

struct HashMap {
  Node* elems;
  uint32 size;
  uint32 capacity;
  uint32 mask;
  uint32 keySize;
  uint32 maxProbe;
};

inline static uint64 Hash (void const* key, uint32 len) {
  return Hash_XX64(key, len, 0x0ULL);
}

HashMap* HashMap_Create (uint32 keySize, uint32 capacity) {
  uint32 logCapacity = 0;
  while (capacity > 1) {
    capacity /= 2;
    logCapacity++;
  }
  capacity = 1 << logCapacity;

  HashMap* self = MemNew(HashMap);
  self->elems = MemNewArrayZero(Node, capacity);
  self->size = 0;
  self->capacity = capacity;
  self->mask = (1 << logCapacity) - 1;
  self->keySize = keySize;
  self->maxProbe = logCapacity;
  return self;
}

void HashMap_Free (HashMap* self) {
  MemFree(self->elems);
  MemFree(self);
}

void HashMap_Foreach (HashMap* self, ValueForeach fn, void* userData) {
  for (uint32 i = 0; i < self->capacity; ++i) {
    Node* node = self->elems + i;
    if (node->value)
      fn(node->value, userData);
  }
}

void* HashMap_Get (HashMap* self, void const* key) {
  return HashMap_GetRaw(self, Hash(key, self->keySize));
}

void* HashMap_GetRaw (HashMap* self, uint64 hash) {
  uint32 index = 0;
  Node* node = self->elems + ((hash + index) & self->mask);
  while (node->value && index < self->maxProbe) {
    if (node->hash == hash) return node->value;
    index++;
    node = self->elems + ((hash + index) & self->mask);
  }
  return 0;
}

/* TODO : Removal in open-addressing is non-trivial! */
#if 0
void* HashMap_Remove (HashMap* self, void const* key) {
  Node* node = HashMap_GetNode(self, key);
  Node* head = node;
  Node* last = 0;

  while (node && node->key) {
    if (memcmp(node->key, key, self->keySize) == 0) {
      void* value = node->value;
      MemFree(node->key);
      if (last) {
        last->next = node->next;
        MemFree(node);
      } else if (node->next)
        *head = *node->next;
      return value;
    }
    last = node;
    node = node->next;
  }

  return 0;
}
#endif

void HashMap_Resize (HashMap* self, uint32 capacity) {
  HashMap* other = HashMap_Create(self->keySize, capacity);
  for (uint32 i = 0; i < self->capacity; ++i) {
    Node* node = self->elems + i;
    if (node->value)
      HashMap_SetRaw(other, node->hash, node->value);
  }

  MemFree(self->elems);
  *self = *other;
}

void HashMap_Set (HashMap* self, void const* key, void* value) {
  HashMap_SetRaw(self, Hash(key, self->keySize), value);
}

void HashMap_SetRaw (HashMap* self, uint64 hash, void* value) {
  uint32 index = 0;
  Node* node = self->elems + ((hash + index) & self->mask);
  while (node->value && index < self->maxProbe) {
    if (node->hash == hash) { node->value = value; return; }
    index++;
    node = self->elems + ((hash + index) & self->mask);
  }

  if (index >= self->maxProbe) {
    HashMap_Resize(self, self->capacity * 2);
    HashMap_SetRaw(self, hash, value);
  } else {
    node->hash = hash;
    node->value = value;
    self->size++;
  }
}

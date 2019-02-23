#include "Hash.h"
#include "PhxMemory.h"
#include "StrMap.h"
#include "PhxString.h"

/* TODO : More rigorous tests for this structure, which has been the victim
 *        of some pretty serious implementation bugs in the past. */

struct Node {
  cstr key;
  Node* next;
  void* value;
};

struct StrMap {
  uint32 capacity;
  uint32 size;
  Node* data;
};

struct StrMapIter {
  StrMap* map;
  Node* node;
  uint32 slot;
};

inline static uint64 Hash (cstr key) {
  return Hash_XX64(key, StrLen(key), 0x0ULL);
}

inline static Node* StrMap_GetBucket (StrMap* self, cstr key) {
  return self->data + (Hash(key) % self->capacity);
}

static void StrMap_Grow (StrMap* self) {
  StrMap newMap = { self->capacity * 2, 0, 0 };
  newMap.data = MemNewArrayZero(Node, newMap.capacity);

  for (uint32 i = 0; i < self->capacity; ++i) {
    Node* node = self->data + i;
    if (!node->key)
      continue;
    StrMap_Set(&newMap, node->key, node->value);
    StrFree(node->key);
    node = node->next;
    while (node) {
      Node* next = node->next;
      StrMap_Set(&newMap, node->key, node->value);
      StrFree(node->key);
      MemFree(node);
      node = next;
    }
  }

  MemFree(self->data);
  *self = newMap;
}

StrMap* StrMap_Create (uint32 capacity) {
  StrMap* self = MemNewZero(StrMap);
  self->capacity = capacity;
  self->data = MemNewArrayZero(Node, capacity);
  return self;
}

void StrMap_Free (StrMap* self) {
  for (uint32 i = 0; i < self->capacity; ++i) {
    Node* node = self->data + i;
    if (!node->key) continue;
    StrFree(node->key);
    node = node->next;
    while (node) {
      Node* next = node->next;
      StrFree(node->key);
      MemFree(node);
      node = next;
    }
  }
  MemFree(self->data);
  MemFree(self);
}

void StrMap_FreeEx (StrMap* self, void (*freeFn)(cstr, void*)) {
  for (uint32 i = 0; i < self->capacity; ++i) {
    Node* node = self->data + i;
    if (!node->key) continue;
    freeFn(node->key, node->value);
    StrFree(node->key);
    node = node->next;
    while (node) {
      Node* next = node->next;
      freeFn(node->key, node->value);
      StrFree(node->key);
      MemFree(node);
      node = next;
    }
  }
  MemFree(self->data);
  MemFree(self);
}

void* StrMap_Get (StrMap* self, cstr key) {
  Node* node = StrMap_GetBucket(self, key);
  if (!node->key)
    return 0;
  while (node) {
    if (StrEqual(node->key, key))
      return node->value;
    node = node->next;
  }
  return 0;
}

uint32 StrMap_GetSize (StrMap* self) {
  return self->size;
}

void StrMap_Remove (StrMap* self, cstr key) {
  Node** prev = 0;
  Node* node = StrMap_GetBucket(self, key);

  while (node && node->key) {
    if (StrEqual(node->key, key)) {
      StrFree(node->key);
      Node* next = node->next;
      if (next) {
        node->key = next->key;
        node->next = next->next;
        node->value = next->value;
        MemFree(next);
      } else {
        node->key = 0;
        node->value = 0;
      }

      if (prev)
        *prev = next;
      return;
    }

    prev = &node->next;
    node = node->next;
  }

  Fatal("StrMap_Remove: Map does not contain key <%s>", key);
}

void StrMap_Set (StrMap* self, cstr key, void* value) {
  self->size++;
  if (3 * self->capacity < 4 * self->size)
    StrMap_Grow(self);

  Node* node = StrMap_GetBucket(self, key);
  if (!node->key) {
    node->key = StrDup(key);
    node->value = value;
    return;
  }

  Node** prev = 0;
  while (node) {
    if (StrEqual(node->key, key)) {
      node->value = value;
      return;
    }
    prev = &node->next;
    node =  node->next;
  }

  node = MemNew(Node);
  node->key = StrDup(key);
  node->value = value;
  node->next = 0;
  *prev = node;
}

#include <stdio.h>

void StrMap_Dump (StrMap* self) {
  printf("StrMap @ %p:\n", self);
  printf("      size: %d\n", self->size);
  printf("  capacity: %d\n", self->capacity);
  printf("      load: %f\n", (float)self->size / (float)self->capacity);
  puts("");

  for (uint32 i = 0; i < self->capacity; ++i) {
    Node* node = self->data + i;
    if (!node->key) continue;
    printf("  [%03i]:\n", i);
    while (node) {
      printf("    (%lx) %s -> %p\n", Hash(node->key), node->key, node->value);
      node = node->next;
    }
  }
}

StrMapIter* StrMap_Iterate (StrMap* self) {
  StrMapIter* it = MemNew(StrMapIter);
  it->map = self;
  it->slot = 0;
  it->node = 0;

  for (uint32 i = 0; i < self->capacity; ++i) {
    Node* node = self->data + i;
    if (!node->key) continue;
    it->slot = i;
    it->node = node;
    break;
  }

  return it;
}

void StrMapIter_Free (StrMapIter* self) {
  MemFree(self);
}

void StrMapIter_Advance (StrMapIter* it) {
  StrMap* self = it->map;
  it->node = it->node->next;
  if (it->node) return;
  it->slot++;
  for (uint32 i = it->slot; i < self->capacity; ++i) {
    Node* node = self->data + i;
    if (!node->key) continue;
    it->slot = i;
    it->node = node;
    break;
  }
}

bool StrMapIter_HasMore (StrMapIter* it) {
  return it->node != 0;
}

cstr StrMapIter_GetKey (StrMapIter* it) {
  return it->node->key;
}

void* StrMapIter_GetValue (StrMapIter* it) {
  return it->node->value;
}

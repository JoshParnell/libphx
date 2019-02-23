#include "PhxMemory.h"
#include "StrBuffer.h"
#include "PhxString.h"

/* NOTE : In order to ensure null-termination of the internal buffer, we always
 *        allocate one byte in excess of the capacity. Hence, data[capacity] is
 *        actually in-bounds, and should always be zeroed. Furthermore, any
 *        bytes beyond size (i.e., data + size and beyond) are kept zeroed for
 *        convenience. */

/* TODO : Fuzz test / validate correctness. This API has not been rigorously
 *        tested, and is low-level enough to contain subtle mistakes. */

struct StrBuffer {
  char*  data;
  uint32 size;
  uint32 capacity;
};

inline static void StrBuffer_GrowTo (StrBuffer* self, uint32 newSize) {
  IF_UNLIKELY (newSize > self->capacity) {
    while (self->capacity < newSize) self->capacity *= 2;
    self->data = (char*)MemRealloc(self->data, self->capacity + 1);
    MemSet(self->data + self->size, 0, (self->capacity + 1) - self->size);
  }
}

inline static void StrBuffer_AppendData (StrBuffer* self, void const* data, uint32 len) {
  StrBuffer_GrowTo(self, self->size + len);
  MemCpy(self->data + self->size, data, len);
  self->size += len;
}

StrBuffer* StrBuffer_Create (uint32 capacity) {
  StrBuffer* self = MemNew(StrBuffer);
  self->data = (char*)MemAllocZero(capacity + 1);
  self->size = 0;
  self->capacity = capacity;
  return self;
}

StrBuffer* StrBuffer_FromStr (char const* s) {
  uint32 len = (uint32)StrLen(s);
  StrBuffer* self = StrBuffer_Create(len);
  self->size = len;
  MemCpy(self->data, s, len);
  return self;
}

void StrBuffer_Free (StrBuffer* self) {
  MemFree(self->data);
  MemFree(self);
}

void StrBuffer_Append (StrBuffer* self, StrBuffer* other) {
  StrBuffer_AppendData(self, (void const*)other->data, other->size);
}

void StrBuffer_AppendStr (StrBuffer* self, char const* other) {
  StrBuffer_AppendData(self, (void const*)other, StrLen(other));
}

inline static int32 StrBuffer_SetImpl (StrBuffer* self, cstr format, va_list args) {
  int32 newSize = vsnprintf(self->data, self->capacity + 1, format, args);
  Assert(newSize >= 0);

  IF_LIKELY ((uint32) newSize <= self->capacity) {
    self->size = newSize;
    return 0;
  } else {
    return newSize - self->capacity;
  }
}

void StrBuffer_Set (StrBuffer* self, cstr format, ...) {
  va_list args;
  va_start(args, format);

  int32 neededSpace = StrBuffer_SetImpl(self, format, args);
  IF_UNLIKELY (neededSpace > 0) {
    StrBuffer_GrowTo(self, self->capacity + neededSpace);

    va_list args2;
    va_start(args2, format);

    neededSpace = StrBuffer_SetImpl(self, format, args);
    Assert(neededSpace == 0);

    va_end(args2);
  }

  va_end(args);
}

StrBuffer* StrBuffer_Clone (StrBuffer* other) {
  StrBuffer* self = StrBuffer_Create(other->size);
  MemCpy(self->data, other->data, other->size);
  self->size = other->size;
  return self;
}

char const* StrBuffer_GetData (StrBuffer* self) {
  return self->data;
}

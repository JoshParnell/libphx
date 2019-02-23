#include "Bytes.h"
#include "File.h"
#include "PhxMemory.h"
#include "PhxString.h"
#include "lz4/lz4.h"

#include <stdio.h>

struct Bytes {
  uint32 size;
  uint32 cursor;
  char data;
};

static void Bytes_CheckLZ4Version () {
  /* liblz4 mandates that only major version number can influence API
   * compatibility. The major version is given by version / (100 * 100). */
  int vLink = LZ4_versionNumber() / (100 * 100);
  int vCompile = (LZ4_VERSION_NUMBER) / (100 * 100);
  if (vLink != vCompile)
    Fatal("Bytes_CheckLZ4Version: Linked against incompatible major version of liblz4:"
          " Compiled (Major): %d, Linked (Major): %d", vCompile, vLink);
}

Bytes* Bytes_Create (uint32 size) {
  Bytes* self = (Bytes*)MemAlloc(2 * sizeof(uint32) + size);
  self->size = size;
  self->cursor = 0;
  return self;
}

Bytes* Bytes_FromData (void const* data, uint32 len) {
  Bytes* self = Bytes_Create(len);
  Bytes_Write(self, data, len);
  Bytes_Rewind(self);
  return self;
}

Bytes* Bytes_Load (cstr path) {
  Bytes* self = File_ReadBytes(path);
  if (!self)
    Fatal("Bytes_Load: Failed to read file '%s'", path);
  return self;
}

void Bytes_Free (Bytes* self) {
  MemFree(self);
}

void* Bytes_GetData (Bytes* self) {
  return &self->data;
}

uint32 Bytes_GetSize (Bytes* self) {
  return self->size;
}

Bytes* Bytes_Compress (Bytes* bytes) {
  Bytes_CheckLZ4Version();
  char* input = (char*)Bytes_GetData(bytes);
  uint32 inputLen = Bytes_GetSize(bytes);
  uint32 header = inputLen;
  uint32 headerLen = sizeof(header);

  if (inputLen > LZ4_MAX_INPUT_SIZE || inputLen > (UINT32_MAX - headerLen))
    Fatal("Bytes_Compress: Input is too large to compress.");

  uint32 bufferLen = inputLen + headerLen;
  char* buffer = MemNewArray(char, bufferLen);
  *(uint32*)buffer = header;

  /* @NOTE: I'm not bothering to use LZ4_compressBound to reduce the size of the
   * temp allocation. Due to the copy below, we're going to immediately throw the
   * memory away, so why waste time calculating how much space is needed? */

  uint32 resultLen = LZ4_compress_default(input, buffer + headerLen, inputLen, bufferLen - headerLen);
  if (resultLen == 0)
    Fatal("Bytes_Compress: LZ4 failed to compress.");

  /* @OPTIMIZE: This is an entire buffer copy that could be avoided. */
  Bytes* result = Bytes_FromData(buffer, resultLen + headerLen);
  MemFree(buffer);
  return result;
}

Bytes* Bytes_Decompress (Bytes* bytes) {
  Bytes_CheckLZ4Version();

  char* input = (char*)Bytes_GetData(bytes);
  uint32 inputLen = Bytes_GetSize(bytes);

  uint32 header = *((uint32*)input);
  uint32 headerLen = sizeof(header);
  uint32 bufferLen = header;
  char* buffer = MemNewArray(char, bufferLen);

  if (inputLen < headerLen)
    Fatal("Bytes_Decompress: Input is smaller than the header size. Data is likely corrupted.");

  int32 resultLen = LZ4_decompress_fast(input + headerLen, buffer, bufferLen);
  if (resultLen < 0)
    Fatal("Bytes_Decompress: LZ4 failed with return value: %i", resultLen);

  if (resultLen + headerLen != inputLen)
    Fatal("Bytes_Decompress: Decompressed length does not match expected result."
          "Expected: %u, Actual: %u", inputLen - headerLen, resultLen);

  /* @OPTIMIZE: This is an entire buffer copy that could be avoided. */
  Bytes* result = Bytes_FromData(buffer, bufferLen);
  MemFree(buffer);
  return result;
}

uint32 Bytes_GetCursor (Bytes* self) {
  return self->cursor;
}

void Bytes_Rewind (Bytes* self) {
  self->cursor = 0;
}

void Bytes_SetCursor (Bytes* self, uint32 cursor) {
  self->cursor = cursor;
}

void Bytes_Read (Bytes* self, void* data, uint32 len) {
  MemCpy(data, &self->data + self->cursor, len);
  self->cursor += len;
}

void Bytes_Write (Bytes* self, void const* data, uint32 len) {
  MemCpy(&self->data + self->cursor, data, len);
  self->cursor += len;
}

void Bytes_WriteStr (Bytes* self, cstr data) {
  size_t len = StrLen(data);
  MemCpy(&self->data + self->cursor, data, len);
  self->cursor += (uint32)len;
}

#define READWRITE_X                                                            \
  X(uint8,   U8)                                                               \
  X(uint16, U16)                                                               \
  X(uint32, U32)                                                               \
  X(uint64, U64)                                                               \
  X(int8,    I8)                                                               \
  X(int16,  I16)                                                               \
  X(int32,  I32)                                                               \
  X(int64,  I64)                                                               \
  X(float,  F32)                                                               \
  X(double, F64)

#define X(T, N)                                                                \
  T Bytes_Read##N(Bytes* self) {                                               \
    T value = *(T*)(&self->data + self->cursor);                               \
    self->cursor += (uint32)sizeof(T);                                         \
    return value;                                                              \
  }                                                                            \
                                                                               \
  void Bytes_Write##N(Bytes* self, T value) {                                  \
    *(T*)(&self->data + self->cursor) = value;                                 \
    self->cursor += (uint32)sizeof(T);                                         \
  }

READWRITE_X
#undef X

void Bytes_Print (Bytes* self) {
  printf("%d bytes:\n", self->size);
  for (uint32 i = 0; i < self->size; ++i)
    putchar(*(&self->data + i));
}

void Bytes_Save (Bytes* self, cstr path) {
  File* file = File_Create(path);
  if (!file)
    Fatal("Bytes_Save: Failed to open file '%s' for writing", path);
  File_Write(file, &self->data, self->size);
  File_Close(file);
}

#include "Bytes.h"
#include "File.h"
#include "PhxMemory.h"
#include "PhxString.h"

#include <stdio.h>

#if WINDOWS
  #include <sys/stat.h>
#elif POSIX
  #include <sys/stat.h>
#endif

struct File {
  FILE* handle;
};

bool File_Exists (cstr path) {
  FILE* f = fopen(path, "rb");
  if (f) {
    fclose(f);
    return true;
  }
  return false;
}

bool File_IsDir (cstr path) {
  struct stat s;
  return stat(path, &s) == 0 && (s.st_mode & S_IFDIR);
}

static File* File_OpenMode (cstr path, cstr mode) {
  FILE* handle = fopen(path, mode);
  if (!handle)
    return 0;
  File* self = MemNew(File);
  self->handle = handle;
  return self;
}

File* File_Create (cstr path) {
  return File_OpenMode(path, "wb");
}

File* File_Open (cstr path) {
  return File_OpenMode(path, "ab");
}

void File_Close (File* self) {
  fclose(self->handle);
  MemFree(self);
}

Bytes* File_ReadBytes (cstr path) {
  FILE* file = fopen(path, "rb");
  if (!file)
    return 0;

  fseek(file, 0, SEEK_END);
  int64 size = (int64)ftell(file);
  if (size == 0) return 0;
  if (size < 0)
    Fatal("File_Read: failed to get size of file '%s'", path);
  rewind(file);

  if (size > UINT32_MAX)
    Fatal("File_Read: filesize of '%s' exceeds 32-bit capacity limit", path);

  Bytes* buffer = Bytes_Create((uint32)size);
  size_t result = fread(Bytes_GetData(buffer), (size_t)size, 1, file);
  if (result != 1)
    Fatal("File_Read: failed to read correct number of bytes from '%s'", path);
  fclose(file);
  return buffer;
}

cstr File_ReadCstr (cstr path) {
  FILE* file = fopen(path, "rb");
  if (!file)
    return 0;

  fseek(file, 0, SEEK_END);
  int64 size = (int64)ftell(file);
  if (size == 0) return 0;
  if (size < 0)
    Fatal("File_ReadAscii: failed to get size of file '%s'", path);
  rewind(file);

  char* buffer = MemNewArray(char, (size_t)size + 1);
  size_t result = fread(buffer, (size_t)size, 1, file);
  if (result != 1)
    Fatal("File_Read: failed to read correct number of bytes from '%s'", path);
  fclose(file);
  buffer[size] = 0;
  return buffer;
}

int64 File_Size (cstr path) {
  FILE* file = fopen(path, "rb");
  if (!file)
    return 0;

  fseek(file, 0, SEEK_END);
  int64 size = (int64)ftell(file);
  fclose(file);
  return size;
}

void File_Read (File* self, void* data, uint32 len) {
  fread(data, len, 1, self->handle);
}

void File_Write (File* self, void const* data, uint32 len) {
  fwrite(data, len, 1, self->handle);
}

void File_WriteStr (File* self, cstr data) {
  fwrite((void const*)data, StrLen(data), 1, self->handle);
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
  T File_Read##N(File* self) {                                                 \
    T value;                                                                   \
    fread((void*)&value, sizeof(T), 1, self->handle);                          \
    return value;                                                              \
  }                                                                            \
                                                                               \
  void File_Write##N(File* self, T value) {                                    \
    fwrite((void const*)&value, sizeof(T), 1, self->handle);                   \
  }

READWRITE_X
#undef X

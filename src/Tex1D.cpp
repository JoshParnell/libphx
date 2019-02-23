#include "Bytes.h"
#include "DataFormat.h"
#include "PhxMemory.h"
#include "OpenGL.h"
#include "PixelFormat.h"
#include "RefCounted.h"
#include "Tex1D.h"
#include "TexFormat.h"

struct Tex1D {
  RefCounted;
  uint handle;
  int size;
  TexFormat format;
};

inline static void Tex1D_Init () {
  GLCALL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST))
  GLCALL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST))
  GLCALL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE))
}

Tex1D* Tex1D_Create (int size, TexFormat format) {
  if (!TexFormat_IsValid(format))
    Fatal("Tex1D_Create: Invalid texture format requested");

  Tex1D* self = MemNew(Tex1D);
  RefCounted_Init(self);
  self->size = size;
  self->format = format;

  GLCALL(glGenTextures(1, &self->handle))
  GLCALL(glActiveTexture(GL_TEXTURE0))
  GLCALL(glBindTexture(GL_TEXTURE_1D, self->handle))
  GLCALL(glTexImage1D(GL_TEXTURE_1D, 0,
    self->format,
    self->size,
    0,
    TexFormat_IsColor(format) ? GL_RED : GL_DEPTH_COMPONENT,
    GL_UNSIGNED_BYTE, 0))
  Tex1D_Init();
  GLCALL(glBindTexture(GL_TEXTURE_1D, 0))
  return self;
}

void Tex1D_Acquire (Tex1D* self) {
  RefCounted_Acquire(self);
}

void Tex1D_Free (Tex1D* self) {
  RefCounted_Free(self) {
    GLCALL(glDeleteTextures(1, &self->handle))
    MemFree(self);
  }
}

void Tex1D_Draw (Tex1D* self, float x, float y, float xs, float ys) {
  GLCALL(glEnable(GL_TEXTURE_1D))
  GLCALL(glBindTexture(GL_TEXTURE_1D, self->handle))
  glBegin(GL_QUADS);
  glTexCoord1f(0);
  glVertex2f(x, y);
  glVertex2f(x, y + ys);
  glTexCoord1f(1);
  glVertex2f(x + xs, y + ys);
  glVertex2f(x + xs, y);
  GLCALL(glEnd())
  GLCALL(glDisable(GL_TEXTURE_1D))
}

void Tex1D_GenMipmap (Tex1D* self) {
  GLCALL(glBindTexture(GL_TEXTURE_1D, self->handle))
  GLCALL(glGenerateMipmap(GL_TEXTURE_1D))
  GLCALL(glBindTexture(GL_TEXTURE_1D, 0))
}

TexFormat Tex1D_GetFormat (Tex1D* self) {
  return self->format;
}

void Tex1D_GetData (Tex1D* self, void* data, PixelFormat pf, DataFormat df) {
  GLCALL(glBindTexture(GL_TEXTURE_1D, self->handle))
  GLCALL(glGetTexImage(GL_TEXTURE_1D, 0, pf, df, data))
  GLCALL(glBindTexture(GL_TEXTURE_1D, 0))
}

Bytes* Tex1D_GetDataBytes (Tex1D* self, PixelFormat pf, DataFormat df) {
  int size = self->size * DataFormat_GetSize(df) * PixelFormat_Components(pf);
  Bytes* data = Bytes_Create(size);
  Tex1D_GetData(self, Bytes_GetData(data), pf, df);
  Bytes_Rewind(data);
  return data;
}

uint Tex1D_GetHandle (Tex1D* self) {
  return self->handle;
}

uint Tex1D_GetSize (Tex1D* self) {
  return self->size;
}

void Tex1D_SetData (Tex1D* self, void const* data, PixelFormat pf, DataFormat df) {
  GLCALL(glBindTexture(GL_TEXTURE_1D, self->handle))
  GLCALL(glTexImage1D(GL_TEXTURE_1D, 0, self->format, self->size, 0, pf, df, data))
  GLCALL(glBindTexture(GL_TEXTURE_1D, 0))
}

void Tex1D_SetDataBytes (Tex1D* self, Bytes* data, PixelFormat pf, DataFormat df) {
  Tex1D_SetData(self, Bytes_GetData(data), pf, df);
}

void Tex1D_SetMagFilter (Tex1D* self, TexFilter filter) {
  GLCALL(glBindTexture(GL_TEXTURE_1D, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, filter))
  GLCALL(glBindTexture(GL_TEXTURE_1D, 0))
}

void Tex1D_SetMinFilter (Tex1D* self, TexFilter filter) {
  GLCALL(glBindTexture(GL_TEXTURE_1D, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, filter))
  GLCALL(glBindTexture(GL_TEXTURE_1D, 0))
}

void Tex1D_SetTexel (Tex1D* self, int x, float r, float g, float b, float a) {
  float rgba[] = { r, g, b, a };
  GLCALL(glBindTexture(GL_TEXTURE_1D, self->handle))
  GLCALL(glTexSubImage1D(GL_TEXTURE_1D, 0, x, 1, GL_RGBA, GL_FLOAT, rgba))
  GLCALL(glBindTexture(GL_TEXTURE_1D, 0))
}

void Tex1D_SetWrapMode (Tex1D* self, TexWrapMode mode) {
  GLCALL(glBindTexture(GL_TEXTURE_1D, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, mode))
  GLCALL(glBindTexture(GL_TEXTURE_1D, 0))
}

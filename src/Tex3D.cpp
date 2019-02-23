#include "Bytes.h"
#include "DataFormat.h"
#include "PhxMemory.h"
#include "OpenGL.h"
#include "PixelFormat.h"
#include "RefCounted.h"
#include "RenderTarget.h"
#include "Tex3D.h"
#include "TexFormat.h"
#include "Vec3.h"

struct Tex3D {
  RefCounted;
  uint handle;
  Vec3i size;
  TexFormat format;
};

inline static void Tex3D_Init () {
  GLCALL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST))
  GLCALL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST))
  GLCALL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE))
  GLCALL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE))
  GLCALL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE))
}

Tex3D* Tex3D_Create (int sx, int sy, int sz, TexFormat format) {
  if (!TexFormat_IsValid(format))
    Fatal("Tex3D_Create: Invalid texture format requested");
  if (TexFormat_IsDepth(format))
    Fatal("Tex3D_Create: Cannot create 3D texture with depth format");

  Tex3D* self = MemNew(Tex3D);
  RefCounted_Init(self);
  self->size = Vec3i_Create(sx, sy, sz);
  self->format = format;

  GLCALL(glGenTextures(1, &self->handle))
  GLCALL(glActiveTexture(GL_TEXTURE0))
  GLCALL(glBindTexture(GL_TEXTURE_3D, self->handle))
  GLCALL(glTexImage3D(GL_TEXTURE_3D, 0,
    self->format,
    self->size.x,
    self->size.y,
    self->size.z,
    0, GL_RED, GL_UNSIGNED_BYTE, 0))
  Tex3D_Init();
  GLCALL(glBindTexture(GL_TEXTURE_3D, 0))
  return self;
}

void Tex3D_Acquire (Tex3D* self) {
  RefCounted_Acquire(self);
}

void Tex3D_Free (Tex3D* self) {
  RefCounted_Free(self) {
    GLCALL(glDeleteTextures(1, &self->handle))
    MemFree(self);
  }
}

void Tex3D_Pop(Tex3D*) {
  RenderTarget_Pop();
}

void Tex3D_Push(Tex3D* self, int layer) {
  RenderTarget_PushTex3D(self, layer);
}

void Tex3D_PushLevel(Tex3D* self, int layer, int level) {
  RenderTarget_PushTex3DLevel(self, layer, level);
}

void Tex3D_Draw (Tex3D* self, int layer, float x, float y, float xs, float ys) {
  float r = (float)(layer + 1) / (float)(self->size.z + 1);
  GLCALL(glEnable(GL_TEXTURE_3D))
  GLCALL(glBindTexture(GL_TEXTURE_3D, self->handle))
  glBegin(GL_QUADS);
  glTexCoord3f(0, 0, r); glVertex2f(x, y);
  glTexCoord3f(0, 1, r); glVertex2f(x, y + ys);
  glTexCoord3f(1, 1, r); glVertex2f(x + xs, y + ys);
  glTexCoord3f(1, 0, r); glVertex2f(x + xs, y);
  GLCALL(glEnd())
  GLCALL(glDisable(GL_TEXTURE_3D))
}

void Tex3D_GenMipmap (Tex3D* self) {
  GLCALL(glBindTexture(GL_TEXTURE_3D, self->handle))
  GLCALL(glGenerateMipmap(GL_TEXTURE_3D))
  GLCALL(glBindTexture(GL_TEXTURE_3D, 0))
}

void Tex3D_GetData (Tex3D* self, void* data, PixelFormat pf, DataFormat df) {
  GLCALL(glBindTexture(GL_TEXTURE_3D, self->handle))
  GLCALL(glGetTexImage(GL_TEXTURE_3D, 0, pf, df, data))
  GLCALL(glBindTexture(GL_TEXTURE_3D, 0))
}

Bytes* Tex3D_GetDataBytes (Tex3D* self, PixelFormat pf, DataFormat df) {
  int size = self->size.x * self->size.y * self->size.z;
  size *= DataFormat_GetSize(df);
  size *= PixelFormat_Components(pf);
  Bytes* data = Bytes_Create(size);
  Tex3D_GetData(self, Bytes_GetData(data), pf, df);
  Bytes_Rewind(data);
  return data;
}

TexFormat Tex3D_GetFormat (Tex3D* self) {
  return self->format;
}

uint Tex3D_GetHandle (Tex3D* self) {
  return self->handle;
}

void Tex3D_GetSize (Tex3D* self, Vec3i* out) {
  *out = self->size;
}

void Tex3D_GetSizeLevel (Tex3D* self, Vec3i* out, int level) {
  *out = self->size;
  for (int i = 0; i < level; ++i) {
    out->x /= 2;
    out->y /= 2;
    out->z /= 2;
  }
}

void Tex3D_SetData (Tex3D* self, void const* data, PixelFormat pf, DataFormat df) {
  GLCALL(glBindTexture(GL_TEXTURE_3D, self->handle))
  GLCALL(glTexImage3D(GL_TEXTURE_3D,
    0, self->format,
    self->size.x, self->size.y, self->size.z,
    0, pf, df, data))
  GLCALL(glBindTexture(GL_TEXTURE_3D, 0))
}

void Tex3D_SetDataBytes (Tex3D* self, Bytes* data, PixelFormat pf, DataFormat df) {
  Tex3D_SetData(self, Bytes_GetData(data), pf, df);
}

void Tex3D_SetMagFilter (Tex3D* self, TexFilter filter) {
  GLCALL(glBindTexture(GL_TEXTURE_3D, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter))
  GLCALL(glBindTexture(GL_TEXTURE_3D, 0))
}

void Tex3D_SetMinFilter (Tex3D* self, TexFilter filter) {
  GLCALL(glBindTexture(GL_TEXTURE_3D, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter))
  GLCALL(glBindTexture(GL_TEXTURE_3D, 0))
}

void Tex3D_SetWrapMode (Tex3D* self, TexWrapMode mode) {
  GLCALL(glBindTexture(GL_TEXTURE_3D, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, mode))
  GLCALL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, mode))
  GLCALL(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, mode))
  GLCALL(glBindTexture(GL_TEXTURE_3D, 0))
}

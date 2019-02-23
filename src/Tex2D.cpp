#include "Bytes.h"
#include "DataFormat.h"
#include "Draw.h"
#include "PhxMemory.h"
#include "Metric.h"
#include "OpenGL.h"
#include "PixelFormat.h"
#include "RefCounted.h"
#include "RenderTarget.h"
#include "Resource.h"
#include "PhxMath.h"
#include "PhxString.h"
#include "Tex2D.h"
#include "TexFormat.h"
#include "Vec2.h"
#include "Viewport.h"

extern uchar* Tex2D_LoadRaw(cstr path, int* sx, int* sy, int* components);
extern bool Tex2D_Save_Png(cstr path, int sx, int sy, int components, uchar* data);

struct Tex2D {
  RefCounted;
  uint handle;
  Vec2i size;
  TexFormat format;
};

inline static void Tex2D_Init () {
  GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST))
  GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST))
  GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE))
  GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE))
}

Tex2D* Tex2D_Create (int sx, int sy, TexFormat format) {
  if (!TexFormat_IsValid(format))
    Fatal("Tex2D_Create: Invalid texture format requested");

  Tex2D* self = MemNew(Tex2D);
  RefCounted_Init(self);

  self->size = Vec2i_Create(sx, sy);
  self->format = format;

  GLCALL(glGenTextures(1, &self->handle))
  GLCALL(glActiveTexture(GL_TEXTURE0))
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glTexImage2D(GL_TEXTURE_2D, 0,
    self->format, self->size.x, self->size.y, 0,
    TexFormat_IsColor(format) ? GL_RED : GL_DEPTH_COMPONENT,
    GL_UNSIGNED_BYTE, 0))
  Tex2D_Init();
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
  return self;
}

Tex2D* Tex2D_ScreenCapture () {
  Vec2i size; Viewport_GetSize(&size);
  Tex2D* self = Tex2D_Create(size.x, size.y, TexFormat_RGBA8);
  uint32* buf = MemNewArray(uint32, size.x * size.y);
  Metric_Inc(Metric_Flush);
  GLCALL(glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, buf))
  for (int y = 0; y < size.y / 2; ++y)
  for (int x = 0; x < size.x; ++x)
    Swap(buf[size.x * y + x], buf[size.x * (size.y - y - 1) + x]);

  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, TexFormat_RGBA8, size.x, size.y,
    0, GL_RGBA, GL_UNSIGNED_BYTE, buf))
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
  return self;
}

void Tex2D_Acquire (Tex2D* self) {
  RefCounted_Acquire(self);
}

void Tex2D_Free (Tex2D* self) {
  RefCounted_Free(self) {
    GLCALL(glDeleteTextures(1, &self->handle))
    MemFree(self);
  }
}

void Tex2D_Pop (Tex2D*) {
  RenderTarget_Pop();
}

void Tex2D_Push (Tex2D* self) {
  RenderTarget_PushTex2D(self);
}

void Tex2D_PushLevel (Tex2D* self, int level) {
  RenderTarget_PushTex2DLevel(self, level);
}

void Tex2D_Clear (Tex2D* self, float r, float g, float b, float a) {
  RenderTarget_PushTex2D(self);
  Draw_Clear(r, g, b, a);
  RenderTarget_Pop();
}

Tex2D* Tex2D_Clone (Tex2D* self) {
  Tex2D* clone = Tex2D_Create(self->size.x, self->size.y, self->format);
  RenderTarget_PushTex2D(self);
  GLCALL(glBindTexture(GL_TEXTURE_2D, clone->handle))
  GLCALL(glCopyTexImage2D(GL_TEXTURE_2D, 0, self->format, 0, 0, self->size.x, self->size.y, 0))
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
  RenderTarget_Pop();
  return clone;
}

void Tex2D_Draw (Tex2D* self, float x, float y, float sx, float sy) {
  Metric_AddDrawImm(1, 2, 4);
  GLCALL(glEnable(GL_TEXTURE_2D))
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex2f(x, y);
  glTexCoord2f(0, 1); glVertex2f(x, y + sy);
  glTexCoord2f(1, 1); glVertex2f(x + sx, y + sy);
  glTexCoord2f(1, 0); glVertex2f(x + sx, y);
  GLCALL(glEnd())
  GLCALL(glDisable(GL_TEXTURE_2D))
}

void Tex2D_DrawEx (
  Tex2D* self,
  float x0, float y0,
  float x1, float y1,
  float u0, float v0,
  float u1, float v1)
{
  Metric_AddDrawImm(1, 2, 4);
  GLCALL(glEnable(GL_TEXTURE_2D))
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  glBegin(GL_QUADS);
  glTexCoord2f(u0, v0); glVertex2f(x0, y0);
  glTexCoord2f(u0, v1); glVertex2f(x0, y1);
  glTexCoord2f(u1, v1); glVertex2f(x1, y1);
  glTexCoord2f(u1, v0); glVertex2f(x1, y0);
  GLCALL(glEnd())
  GLCALL(glDisable(GL_TEXTURE_2D))
}

void Tex2D_GenMipmap (Tex2D* self) {
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glGenerateMipmap(GL_TEXTURE_2D))
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
}

void Tex2D_GetData (Tex2D* self, void* data, PixelFormat pf, DataFormat df) {
  Metric_Inc(Metric_Flush);
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glGetTexImage(GL_TEXTURE_2D, 0, pf, df, data))
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
}

Bytes* Tex2D_GetDataBytes (Tex2D* self, PixelFormat pf, DataFormat df) {
  int size = self->size.x * self->size.y;
  size *= DataFormat_GetSize(df);
  size *= PixelFormat_Components(pf);
  Bytes* data = Bytes_Create(size);
  Tex2D_GetData(self, Bytes_GetData(data), pf, df);
  Bytes_Rewind(data);
  return data;
}

TexFormat Tex2D_GetFormat (Tex2D* self) {
  return self->format;
}

uint Tex2D_GetHandle (Tex2D* self) {
  return self->handle;
}

void Tex2D_GetSize (Tex2D* self, Vec2i* out) {
  *out = self->size;
}

void Tex2D_GetSizeLevel (Tex2D* self, Vec2i* out, int level) {
  *out = self->size;
  for (int i = 0; i < level; ++i) {
    out->x /= 2;
    out->y /= 2;
  }
}

Tex2D* Tex2D_Load (cstr name) {
  cstr path = Resource_GetPath(ResourceType_Tex2D, name);
  int sx, sy, components = 4;
  uchar* data = Tex2D_LoadRaw(path, &sx, &sy, &components);
  Tex2D* self = Tex2D_Create(sx, sy, TexFormat_RGBA8);

  GLenum format =
    components == 4 ? GL_RGBA :
    components == 3 ? GL_RGB  :
    components == 2 ? GL_RG : GL_RED;

  GLCALL(glActiveTexture(GL_TEXTURE0))
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glTexImage2D(GL_TEXTURE_2D, 0,
    GL_RGBA8, self->size.x, self->size.y, 0,
    format, GL_UNSIGNED_BYTE, data))
  Tex2D_Init();
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))

  MemFree(data);
  return self;
}

void Tex2D_SetAnisotropy (Tex2D* self, float factor) {
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, factor))
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
}

void Tex2D_SetData (Tex2D* self, void const* data, PixelFormat pf, DataFormat df) {
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glTexImage2D(GL_TEXTURE_2D,
    0, self->format,
    self->size.x, self->size.y,
    0, pf, df, data))
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
}

void Tex2D_SetDataBytes (Tex2D* self, Bytes* data, PixelFormat pf, DataFormat df) {
  Tex2D_SetData(self, Bytes_GetData(data), pf, df);
}

void Tex2D_SetMagFilter (Tex2D* self, TexFilter filter) {
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter))
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
}

void Tex2D_SetMinFilter (Tex2D* self, TexFilter filter) {
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter))
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
}

/* NOTE : In general, using BASE_LEVEL, MAX_LEVEL, and MIN/MAX_LOD params is
 *        dangerous due to known bugs in old Radeon & Intel drivers. See:
 *        (https://www.opengl.org/discussion_boards/showthread.php/
 *         166266-Using-GL_TEXTURE_BASE_LEVEL-with-a-comple-texture)
 *
 *        However, constraining the mip range to a single level (minLevel ==
 *        maxLevel) seems to be acceptable even on bad drivers. Thus, it is
 *        strongly advised to use this function only to constrain sampling to
 *        a single mip level. */
void Tex2D_SetMipRange (Tex2D* self, int minLevel, int maxLevel) {
  if (minLevel != maxLevel)
    Warn("Tex2D_SetMipRange: Setting mip range with min != max; this may fail"
         " on old drivers with mip-handling bugs.");
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, minLevel))
  GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel))
}

void Tex2D_SetTexel (Tex2D* self, int x, int y, float r, float g, float b, float a) {
  float rgba[] = { r, g, b, a };
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RGBA, GL_FLOAT, rgba))
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
}

void Tex2D_SetWrapMode (Tex2D* self, TexWrapMode mode) {
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode))
  GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode))
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
}

void Tex2D_Save (Tex2D* self, cstr path) {
  Metric_Inc(Metric_Flush);
  GLCALL(glBindTexture(GL_TEXTURE_2D, self->handle))
  uchar* buffer = (uchar*)MemAlloc(4 * self->size.x * self->size.y);
  GLCALL(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer))
  Tex2D_Save_Png(path, self->size.x, self->size.y, 4, buffer);
  MemFree(buffer);
  GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
}

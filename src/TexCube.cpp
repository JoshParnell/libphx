#include "BlendMode.h"
#include "Bytes.h"
#include "ClipRect.h"
#include "CubeFace.h"
#include "CullFace.h"
#include "DataFormat.h"
#include "Draw.h"
#include "File.h"
#include "GLMatrix.h"
#include "PhxMemory.h"
#include "OpenGL.h"
#include "PixelFormat.h"
#include "RefCounted.h"
#include "RenderState.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "ShaderState.h"
#include "TimeStamp.h"
#include "PhxString.h"
#include "TexCube.h"
#include "TexFormat.h"

extern uchar* Tex2D_LoadRaw(cstr path, int* sx, int* sy, int* components);
extern bool Tex2D_Save_Png(cstr path, int sx, int sy, int components, uchar* data);

struct Face {
  CubeFace face;
  Vec3f look;
  Vec3f up;
};

static Face const kFaces[] = {
  { CubeFace_PX, { 1,  0,  0}, {0, 1,  0} },
  { CubeFace_NX, {-1,  0,  0}, {0, 1,  0} },
  { CubeFace_PY, { 0,  1,  0}, {0, 0, -1} },
  { CubeFace_NY, { 0, -1,  0}, {0, 0,  1} },
  { CubeFace_PZ, { 0,  0,  1}, {0, 1,  0} },
  { CubeFace_NZ, { 0,  0, -1}, {0, 1,  0} },
};

static cstr const kFaceExt[] = {
  "px", "py", "pz",
  "nx", "ny", "nz",
};

struct TexCube {
  RefCounted;
  uint handle;
  int size;
  TexFormat format;
};

inline static void TexCube_InitParameters () {
  GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST))
  GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST))
  GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE))
  GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE))
}

TexCube* TexCube_Create (int size, TexFormat format) {
  if (!TexFormat_IsValid(format))
    Fatal("TexCube_Create: Invalid texture format requested");
  if (TexFormat_IsDepth(format))
    Fatal("TexCube_Create: Cannot create cubemap with depth format");

  TexCube* self = MemNew(TexCube);
  RefCounted_Init(self);
  GLCALL(glGenTextures(1, &self->handle))
  self->size = size;
  self->format = format;
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, self->handle))
  GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, size, size, 0, GL_RED, GL_BYTE, 0))
  GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, size, size, 0, GL_RED, GL_BYTE, 0))
  GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, size, size, 0, GL_RED, GL_BYTE, 0))
  GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, size, size, 0, GL_RED, GL_BYTE, 0))
  GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, size, size, 0, GL_RED, GL_BYTE, 0))
  GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, size, size, 0, GL_RED, GL_BYTE, 0))
  TexCube_InitParameters();
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0))
  return self;
}

void TexCube_Acquire (TexCube* self) {
  RefCounted_Acquire(self);
}

void TexCube_Clear (TexCube* self, float r, float g, float b, float a) {
  for (int i = 0; i < 6; i++) {
    Face face = kFaces[i];
    RenderTarget_Push(self->size, self->size);
    RenderTarget_BindTexCube(self, face.face);
    Draw_Clear(r, g, b, a);
    RenderTarget_Pop();
  }
}

void TexCube_Free (TexCube* self) {
  RefCounted_Free(self) {
    GLCALL(glDeleteTextures(1, &self->handle))
    MemFree(self);
  }
}

TexCube* TexCube_Load (cstr path) {
  TexCube* self = MemNew(TexCube);
  GLCALL(glGenTextures(1, &self->handle))
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, self->handle))

  int components = 0;
  int dataLayout = 0;

  for (int i = 0; i < 6; ++i) {
    cstr facePath = StrAdd3(path, kFaceExt[i], ".jpg");
    int sx, sy, lcomponents;
    uchar* data = Tex2D_LoadRaw(facePath, &sx, &sy, &lcomponents);
    if (!data)
      Fatal("TexCube_Load failed to load cubemap face from '%s'", facePath);
    if (sx != sy)
      Fatal("TexCube_Load loaded cubemap face is not square");
    if (i) {
      if (sx != self->size || sy != self->size)
        Fatal("TexCube_Load loaded cubemap faces have different resolutions");
      if (lcomponents != components)
        Fatal("TexCube_Load loaded cubemap faces have different number of components");
    } else {
      components = lcomponents;
      self->size = sx;
      self->format =
        components == 4 ? TexFormat_RGBA8 :
        components == 3 ? TexFormat_RGB8 :
        components == 2 ? TexFormat_RG8 : TexFormat_R8;
      dataLayout =
        components == 4 ? GL_RGBA :
        components == 3 ? GL_RGB :
        components == 2 ? GL_RG : GL_RED;
    }

    GLCALL(glTexImage2D(kFaces[i].face, 0, self->format, self->size, self->size, 0, dataLayout, GL_UNSIGNED_BYTE, data))
    MemFree(facePath);
    MemFree(data);
  }

  TexCube_InitParameters();
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0))
  return self;
}

void TexCube_GetData (
  TexCube* self,
  void* data,
  CubeFace face,
  int level,
  PixelFormat pf,
  DataFormat df)
{
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, self->handle))
  GLCALL(glGetTexImage(face, level, pf, df, data))
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0))
}

Bytes* TexCube_GetDataBytes (
  TexCube* self,
  CubeFace face,
  int level,
  PixelFormat pf,
  DataFormat df)
{
  int size = self->size * self->size;
  size *= DataFormat_GetSize(df);
  size *= PixelFormat_Components(pf);
  Bytes* data = Bytes_Create(size);
  TexCube_GetData(self, Bytes_GetData(data), face, level, pf, df);
  Bytes_Rewind(data);
  return data;
}

TexFormat TexCube_GetFormat (TexCube* self) {
  return self->format;
}

uint TexCube_GetHandle (TexCube* self) {
  return self->handle;
}

int TexCube_GetSize (TexCube* self) {
  return self->size;
}

void TexCube_Generate (TexCube* self, ShaderState* state) {
  GLMatrix_ModeP();  GLMatrix_Push(); GLMatrix_Clear();
  GLMatrix_ModeWV(); GLMatrix_Push(); GLMatrix_Clear();
  RenderState_PushAllDefaults();
  ShaderState_Start(state);

  for (int i = 0; i < 6; i++) {
    Face face = kFaces[i];
    int size = self->size;
    float fSize = (float) self->size;
    RenderTarget_Push(size, size);
    RenderTarget_BindTexCube(self, face.face);
    Draw_Clear(0, 0, 0, 1);
    Shader_SetFloat3("cubeLook", UNPACK3(face.look));
    Shader_SetFloat3("cubeUp", UNPACK3(face.up));
    Shader_SetFloat("cubeSize", fSize);

    int j = 1;
    int jobSize = 1;
    while (j <= size) {
      TimeStamp time = TimeStamp_Get();

      ClipRect_Push(0, j - 1, size, jobSize);
      Draw_Rect(0, 0, fSize, fSize);
      Draw_Flush();
      ClipRect_Pop();

      j += jobSize;
      double elapsed = TimeStamp_GetElapsed(time);
      jobSize = Max(1, (int)Floor(0.25 * jobSize / elapsed + 0.5));
      jobSize = Min(jobSize, (size - j + 1));
    }

    RenderTarget_Pop();
  }

  ShaderState_Stop(state);
  RenderState_PopAll();
  GLMatrix_ModeP();  GLMatrix_Pop();
  GLMatrix_ModeWV(); GLMatrix_Pop();
}

void TexCube_GenMipmap (TexCube* self) {
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, self->handle))
  GLCALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP))
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0))
}

void TexCube_SetData (
  TexCube* self,
  void const* data,
  CubeFace face,
  int level,
  PixelFormat pf,
  DataFormat df)
{
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, self->handle))
  GLCALL(glTexImage2D(face, level, self->format, self->size, self->size, 0, pf, df, data))
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0))
}

void TexCube_SetDataBytes (
  TexCube* self,
  Bytes* data,
  CubeFace face,
  int level,
  PixelFormat pf,
  DataFormat df)
{
  TexCube_SetData(self, Bytes_GetData(data), face, level, pf, df);
}

void TexCube_SetMagFilter (TexCube* self, TexFilter filter) {
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter))
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0))
}

void TexCube_SetMinFilter (TexCube* self, TexFilter filter) {
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, self->handle))
  GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter))
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0))
}

void TexCube_Save (TexCube* self, cstr path) {
  TexCube_SaveLevel(self, path, 0);
}

void TexCube_SaveLevel (TexCube* self, cstr path, int level) {
  int size = self->size >> level;
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, self->handle))
  uchar* buffer = MemNewArray(uchar, 4 * size * size);
  for (int i = 0; i < 6; ++i) {
    CubeFace face = kFaces[i].face;
    cstr facePath = StrAdd3(path, kFaceExt[i], ".png");
    GLCALL(glGetTexImage(face, level, GL_RGBA, GL_UNSIGNED_BYTE, buffer))
    Tex2D_Save_Png(facePath, size, size, 4, buffer);
    MemFree(facePath);
  }
  MemFree(buffer);
  GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0))
}

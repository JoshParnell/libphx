#include "Metric.h"
#include "OpenGL.h"
#include "Profiler.h"
#include "RenderTarget.h"
#include "Tex2D.h"
#include "Tex3D.h"
#include "TexCube.h"
#include "TexFormat.h"
#include "Viewport.h"
#include "Vec2.h"
#include "Vec3.h"

/* TODO : FBO caching mechanism to avoid driver-side validation. */

#define MAX_COLOR_ATTACHMENTS 4
#define MAX_STACK_DEPTH 16

struct FBO {
  uint handle;
  int colorIndex;
  int sx, sy;
  bool depth;
};

static int fboIndex = -1;
static FBO fboStack[MAX_STACK_DEPTH];

inline static FBO* GetActive () {
  return fboStack + fboIndex;
}

inline static void SetDrawBuffers (int count) {
  static const GLenum bufs[MAX_COLOR_ATTACHMENTS] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
  };

  GLCALL(glDrawBuffers(count, bufs))
}

void RenderTarget_Push (int sx, int sy) {
  FRAME_BEGIN;
  if (fboIndex + 1 >= MAX_STACK_DEPTH)
    Fatal("RenderTarget_Push: Maximum stack depth exceeded");

  fboIndex++;
  FBO* self = GetActive();
  self->handle = 0;
  self->colorIndex = 0;
  self->sx = sx;
  self->sy = sy;
  self->depth = false;
  Metric_Inc(Metric_FBOSwap);
  GLCALL(glGenFramebuffers(1, &self->handle))
  GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, self->handle))
  Viewport_Push(0, 0, sx, sy, false);
  FRAME_END;
}

void RenderTarget_Pop () {
  FRAME_BEGIN;
  if (fboIndex < 0)
    Fatal("RenderTarget_Pop: Attempting to pop an empty stack");

  /* Delete current FBO. */
  for (int i = 0; i < MAX_COLOR_ATTACHMENTS; ++i)
    GLCALL(glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0 + i,
      GL_TEXTURE_2D, 0, 0))

  GLCALL(glFramebufferTexture2D(
    GL_FRAMEBUFFER,
    GL_DEPTH_ATTACHMENT,
    GL_TEXTURE_2D, 0, 0))

  GLCALL(glDeleteFramebuffers(1, &fboStack[fboIndex].handle))
  fboIndex--;

  /* Activate previous FBO. */
  Metric_Inc(Metric_FBOSwap);
  if (fboIndex >= 0)
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, GetActive()->handle))
  else
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0))

  Viewport_Pop();
  FRAME_END;
}

void RenderTarget_BindTex2D (Tex2D* self) {
  RenderTarget_BindTex2DLevel(self, 0);
}

/* TODO : Check whether it is valid to bind a mip level of a depth buffer?
 *        (Do depth formats even have levels??) */
void RenderTarget_BindTex2DLevel (Tex2D* tex, int level) {
  FBO* self = GetActive();
  uint handle = Tex2D_GetHandle(tex);

  /* Color attachment. */
  if (TexFormat_IsColor(Tex2D_GetFormat(tex))) {
    if (self->colorIndex >= MAX_COLOR_ATTACHMENTS)
      Fatal("RenderTarget_BindTex2D: Max color attachments exceeded");
    GLCALL(glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0 + self->colorIndex++,
      GL_TEXTURE_2D, handle, level))
    SetDrawBuffers(self->colorIndex);
  }

  /* Depth attachment. */
  else {
    if (self->depth)
      Fatal("RenderTarget_BindTex2D: Target already has a depth buffer");
    GLCALL(glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_DEPTH_ATTACHMENT,
      GL_TEXTURE_2D, handle, level))
    self->depth = true;
  }
}

void RenderTarget_BindTex3D (Tex3D* self, int layer) {
  RenderTarget_BindTex3DLevel(self, layer, 0);
}

void RenderTarget_BindTex3DLevel (Tex3D* tex, int layer, int level) {
  FBO* self = GetActive();
  if (self->colorIndex >= MAX_COLOR_ATTACHMENTS)
    Fatal("RenderTarget_BindTex3D: Max color attachments exceeded");

  uint handle = Tex3D_GetHandle(tex);
  GLCALL(glFramebufferTexture3D(
    GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0 + self->colorIndex++,
    GL_TEXTURE_3D, handle, level, layer))
  SetDrawBuffers(self->colorIndex);
}

void RenderTarget_BindTexCube (TexCube* self, CubeFace face) {
  RenderTarget_BindTexCubeLevel(self, face, 0);
}

void RenderTarget_BindTexCubeLevel (TexCube* tex, CubeFace face, int level) {
  FBO* self = GetActive();
  if (self->colorIndex >= MAX_COLOR_ATTACHMENTS)
    Fatal("RenderTarget_BindTexCubeLevel: Max color attachments exceeded");

  uint handle = TexCube_GetHandle(tex);
  GLCALL(glFramebufferTexture2D(
    GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0 + self->colorIndex++,
    face, handle, level))
  SetDrawBuffers(self->colorIndex);
}

void RenderTarget_PushTex2D (Tex2D* self) {
  RenderTarget_PushTex2DLevel(self, 0);
}

void RenderTarget_PushTex2DLevel (Tex2D* self, int level) {
  Vec2i size; Tex2D_GetSizeLevel(self, &size, level);
  RenderTarget_Push(size.x, size.y);
  RenderTarget_BindTex2DLevel(self, level);
}

void RenderTarget_PushTex3D (Tex3D* self, int layer) {
  RenderTarget_PushTex3DLevel(self, layer, 0);
}

void RenderTarget_PushTex3DLevel (Tex3D* self, int layer, int level) {
  Vec3i size; Tex3D_GetSizeLevel(self, &size, level);
  RenderTarget_Push(size.x, size.y);
  RenderTarget_BindTex3DLevel(self, layer, level);
}

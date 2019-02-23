#include "OpenGL.h"
#include "Vec2.h"
#include "Viewport.h"

#define MAX_STACK_DEPTH 16

/* TODO : This is a low-level mechanism and probably not for use outside of
 *        RenderTarget. Should likely be folded into RenderTarget. */

struct VP {
  int x, y, sx, sy;
  bool isWindow;
};

static int vpIndex = -1;
static VP vp[MAX_STACK_DEPTH];

static void Viewport_Set (VP const* self) {
  GLCALL(glViewport(self->x, self->y, self->sx, self->sy))
  GLCALL(glMatrixMode(GL_PROJECTION))
  GLCALL(glLoadIdentity())

  /* GL's window coordinates and texture coordinates have opposite vertical
   * orientation. Automatically compensate via the projection matrix. */
  if (self->isWindow) {
    GLCALL(glTranslatef(-1.0, 1.0, 0.0))
    GLCALL(glScalef(2.0f / self->sx, -2.0f / self->sy, 1.0f))
  } else {
    GLCALL(glTranslatef(-1.0, -1.0, 0.0))
    GLCALL(glScalef(2.0f / self->sx,  2.0f / self->sy, 1.0f))
  }
}

float Viewport_GetAspect () {
  if (vpIndex < 0)
    Fatal("Viewport_GetAspect: Viewport stack is empty");
  return (float)vp[vpIndex].sx / (float)vp[vpIndex].sy;
}

void Viewport_GetSize (Vec2i* out) {
  if (vpIndex < 0)
    Fatal("Viewport_GetSize: Viewport stack is empty");
  out->x = vp[vpIndex].sx;
  out->y = vp[vpIndex].sy;
}

void Viewport_Push (int x, int y, int sx, int sy, bool isWindow) {
  if (vpIndex + 1 >= MAX_STACK_DEPTH)
    Fatal("Viewport_Push: Maximum viewport stack depth exceeded");
  vpIndex++;
  VP* self = vp + vpIndex;
  self->x = x;
  self->y = y;
  self->sx = sx;
  self->sy = sy;
  self->isWindow = isWindow;
  Viewport_Set(self);
}

void Viewport_Pop () {
  if (vpIndex < 0)
    Fatal("Viewport_Pop: Viewport stack is empty");
  vpIndex--;
  if (vpIndex >= 0)
    Viewport_Set(vp + vpIndex);
}

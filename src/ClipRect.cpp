#include "ClipRect.h"
#include "OpenGL.h"
#include "PhxMath.h"
#include "Vec2.h"
#include "Viewport.h"

#define MAX_STACK_DEPTH 128

struct ClipRectTransform {
  float tx, ty;
  float sx, sy;
};

static ClipRectTransform transform[MAX_STACK_DEPTH];
static int transformIndex = -1;

struct ClipRect {
  float x, y;
  float sx, sy;
  bool enabled;
};

static ClipRect rect[MAX_STACK_DEPTH];
static int rectIndex = -1;

inline static void TransformRect (float* x, float* y, float* sx, float* sy) {
  if (transformIndex >= 0) {
    ClipRectTransform* curr = transform + transformIndex;
    *x = curr->sx * (*x) + curr->tx;
    *y = curr->sy * (*y) + curr->ty;
    *sx = curr->sx * (*sx);
    *sy = curr->sy * (*sy);
  }
}

void ClipRect_Activate (ClipRect* self) {
  if (self && self->enabled) {
    Vec2i vpSize;
    Viewport_GetSize(&vpSize);
    GLCALL(glEnable(GL_SCISSOR_TEST))
    float x = self->x;
    float y = self->y;
    float sx = self->sx;
    float sy = self->sy;
    TransformRect(&x, &y, &sx, &sy);
    GLCALL(glScissor(
      (int)x,
      vpSize.y - (int)(y + sy),
      (int)sx,
      (int)sy))
  } else {
    GLCALL(glDisable(GL_SCISSOR_TEST))
  }
}

void ClipRect_Push (float x, float y, float sx, float sy) {
  if (rectIndex + 1 >= MAX_STACK_DEPTH)
    Fatal("ClipRect_Push: Maximum stack depth exceeded");
  rectIndex++;
  ClipRect* curr = rect + rectIndex;
  curr->x = x;
  curr->y = y;
  curr->sx = sx;
  curr->sy = sy;
  curr->enabled = true;
  ClipRect_Activate(curr);
}

void ClipRect_PushCombined (float x, float y, float sx, float sy) {
  ClipRect* curr = rect + rectIndex;
  if (rectIndex >= 0 && curr->enabled) {
    float maxX = x + sx;
    float maxY = y + sy;
    x = Max(x, curr->x);
    y = Max(y, curr->y);

    ClipRect_Push(x, y,
      Min(maxX, curr->x + curr->sx) - x,
      Min(maxY, curr->y + curr->sy) - y
   );
  } else {
    ClipRect_Push(x, y, sx, sy);
  }
}

void ClipRect_PushDisabled () {
  if (rectIndex + 1 >= MAX_STACK_DEPTH)
    Fatal("ClipRect_Push: Maximum stack depth exceeded");
  rectIndex++;
  ClipRect* curr = rect + rectIndex;
  curr->enabled = false;
  ClipRect_Activate(curr);
}

void ClipRect_PushTransform (float tx, float ty, float sx, float sy) {
  if (transformIndex + 1 >= MAX_STACK_DEPTH)
    Fatal("ClipRect_PushTransform: Maximum stack depth exceeded");
  transformIndex++;
  ClipRectTransform* curr = transform + transformIndex;
  curr->tx = tx;
  curr->ty = ty;
  curr->sx = sx;
  curr->sy = sy;
  if (rectIndex >= 0)
    ClipRect_Activate(rect + rectIndex);
}

void ClipRect_Pop () {
  if (rectIndex < 0)
    Fatal("ClipRect_Pop: Attempting to pop an empty stack");
  rectIndex--;
  ClipRect_Activate(rectIndex >= 0 ? rect + rectIndex : 0);
}

void ClipRect_PopTransform () {
  if (transformIndex < 0)
    Fatal("ClipRect_PopTransform: Attempting to pop an empty stack");
  transformIndex--;
  if (rectIndex >= 0)
    ClipRect_Activate(rect + rectIndex);
}

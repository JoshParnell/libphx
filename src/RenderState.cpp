#include "BlendMode.h"
#include "CullFace.h"
#include "OpenGL.h"
#include "RenderState.h"

#define MAX_STACK_DEPTH 16

#define RENDERSTATE_X                                                          \
  X(BlendMode, BlendMode, blendMode)                                           \
  X(CullFace, CullFace, cullFace)                                              \
  X(bool, DepthTest, depthTest)                                                \
  X(bool, DepthWritable, depthWritable)                                        \
  X(bool, Wireframe, wireframe)

#define X(T, StateFn, State)                                                   \
  static T State[MAX_STACK_DEPTH];                                             \
  static int State##Index = -1;
RENDERSTATE_X
#undef X

inline static void RenderState_SetBlendMode (BlendMode mode) {
  switch (mode) {
    case BlendMode_Additive:
      GLCALL(glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE))
      return;
    case BlendMode_Alpha:
      GLCALL(glBlendFuncSeparate(
        GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
        GL_ONE, GL_ONE_MINUS_SRC_ALPHA))
      return;
    case BlendMode_PreMultAlpha:
      GLCALL(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA))
      return;
    case BlendMode_Disabled:
      GLCALL(glBlendFunc(GL_ONE, GL_ZERO))
      return;
  }
}

inline static void RenderState_SetCullFace (CullFace mode) {
  switch (mode) {
    case CullFace_None:
      GLCALL(glDisable(GL_CULL_FACE))
      return;
    case CullFace_Back:
      GLCALL(glEnable(GL_CULL_FACE))
      GLCALL(glCullFace(GL_BACK))
      return;
    case CullFace_Front:
      GLCALL(glEnable(GL_CULL_FACE))
      GLCALL(glCullFace(GL_FRONT))
      return;
  }
}

inline static void RenderState_SetDepthTest (bool enabled) {
  if (enabled)
    GLCALL(glEnable(GL_DEPTH_TEST))
  else
    GLCALL(glDisable(GL_DEPTH_TEST))
}

inline static void RenderState_SetDepthWritable (bool enabled) {
  GLCALL(glDepthMask(enabled))
}

inline static void RenderState_SetWireframe (bool enabled) {
  if (enabled)
    GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE))
  else
    GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL))
}

void RenderState_PushAllDefaults () {
  RenderState_PushBlendMode(BlendMode_Disabled);
  RenderState_PushCullFace(CullFace_None);
  RenderState_PushDepthTest(false);
  RenderState_PushDepthWritable(true);
  RenderState_PushWireframe(false);
}

void RenderState_PopAll () {
  RenderState_PopBlendMode();
  RenderState_PopCullFace();
  RenderState_PopDepthTest();
  RenderState_PopDepthWritable();
  RenderState_PopWireframe();
}

#define X(T, StateFn, State)                                                   \
  void RenderState_Push##StateFn(T value) {                                    \
    if (State##Index + 1 >= MAX_STACK_DEPTH)                                   \
      Fatal("RenderState_Push" #StateFn ": Maximum state stack depth exceeded"); \
    State[++State##Index] = value;                                             \
    RenderState_Set##StateFn(value);                                           \
  }                                                                            \
                                                                               \
  void RenderState_Pop##StateFn() {                                            \
    if (State##Index < 0)                                                      \
      Fatal("RenderState_Pop" #StateFn ": Attempting to pop an empty state stack"); \
    State##Index--;                                                            \
    if (State##Index >= 0)                                                     \
      RenderState_Set##StateFn(State[State##Index]);                           \
  }

RENDERSTATE_X
#undef X

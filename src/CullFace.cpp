#include "CullFace.h"
#include "RenderState.h"

void CullFace_Pop() {
  RenderState_PopCullFace();
}

void CullFace_Push(CullFace cullFace) {
  RenderState_PushCullFace(cullFace);
}

void CullFace_PushNone() {
  RenderState_PushCullFace(CullFace_None);
}

void CullFace_PushBack() {
  RenderState_PushCullFace(CullFace_Back);
}

void CullFace_PushFront() {
  RenderState_PushCullFace(CullFace_Front);
}

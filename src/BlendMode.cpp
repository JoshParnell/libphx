#include "BlendMode.h"
#include "RenderState.h"

void BlendMode_Pop() {
  RenderState_PopBlendMode();
}

void BlendMode_Push(BlendMode blendMode) {
  RenderState_PushBlendMode(blendMode);
}

void BlendMode_PushAdditive() {
  RenderState_PushBlendMode(BlendMode_Additive);
}

void BlendMode_PushAlpha() {
  RenderState_PushBlendMode(BlendMode_Alpha);
}

void BlendMode_PushDisabled() {
  RenderState_PushBlendMode(BlendMode_Disabled);
}

void BlendMode_PushPreMultAlpha() {
  RenderState_PushBlendMode(BlendMode_PreMultAlpha);
}

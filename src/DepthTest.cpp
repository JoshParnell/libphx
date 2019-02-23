#include "DepthTest.h"
#include "RenderState.h"

void DepthTest_Pop() {
  RenderState_PopDepthTest();
}

void DepthTest_Push(bool depthTest) {
  RenderState_PushDepthTest(depthTest);
}

void DepthTest_PushDisabled() {
  RenderState_PushDepthTest(false);
}

void DepthTest_PushEnabled() {
  RenderState_PushDepthTest(true);
}

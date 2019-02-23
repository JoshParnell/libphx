#include "Renderer.h"

struct Renderer {
  int dsFactor;
} self = { 0 };

void Renderer_Init () {
  self.dsFactor = 4;
}

void Renderer_Free () {
}

void Renderer_SetDownsampleFactor (int dsFactor) {
  self.dsFactor = dsFactor;
}

void Renderer_SetShader (Shader* shader) {
}

void Renderer_SetTransform (Matrix* toWorld, Matrix* toLocal) {
}

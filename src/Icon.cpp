#include "BlendMode.h"
#include "Cache.h"
#include "Draw.h"
#include "Icon.h"
#include "PhxMemory.h"
#include "Shader.h"
#include "Tex2D.h"
#include "TexFormat.h"

struct Icon {
  Tex2D* tex;
};

Icon* Icon_Create () {
  Icon* self = MemNew(Icon);
  return self;
}

void Icon_Free (Icon* self) {
  MemFree(self);
}

void Icon_AddBox (Icon* self, float x, float y, float sx, float sy) {
}

void Icon_AddCircle (Icon* self, float x, float y, float radius) {
}

void Icon_AddPoint (Icon* self, float x, float y) {
}

void Icon_AddLine (Icon* self, float x1, float y1, float x2, float y2) {
}

void Icon_AddRing (Icon* self, float x, float y, float radius, float thickness) {
}

void Icon_Draw (
  Icon* self,
  float x, float y, float size,
  float r, float g, float b, float a)
{
}

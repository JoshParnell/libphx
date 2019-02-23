#include "Draw.h"
#include "PhxMemory.h"
#include "Widget.h"
#include "WidgetDef.h"
#include "Vec4.h"

struct WidgetBox : public Widget {
  Vec4f color;

  void onDrawPost ();
};

Widget* Widget_CreateBox (float r, float g, float b, float a) {
  WidgetBox* self = new WidgetBox;
  self->color.x = r;
  self->color.y = g;
  self->color.z = b;
  self->color.w = a;
  self->stretch.x = 1.0f;
  self->stretch.y = 1.0f;
  return self;
}

void WidgetBox::onDrawPost () {
  Draw_Color(color.x, color.y, color.z, color.w);
  Draw_Rect(pos.x, pos.y, size.x, size.y);
}

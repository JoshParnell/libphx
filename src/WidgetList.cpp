#include "Draw.h"
#include "PhxMath.h"
#include "PhxMemory.h"
#include "Widget.h"
#include "WidgetDef.h"

struct WidgetListH : public Widget {
  float spacing;
  void onComputeSize ();
  void onLayout ();
};

struct WidgetListV : public Widget {
  float spacing;
  void onComputeSize ();
  void onLayout ();
};

Widget* Widget_CreateListH (float spacing) {
  WidgetListH* self = new WidgetListH;
  self->spacing = spacing;
  self->stretch.y = 1.0f;
  return self;
}

Widget* Widget_CreateListV (float spacing) {
  WidgetListV* self = new WidgetListV;
  self->spacing = spacing;
  self->stretch.x = 1.0f;
  return self;
}

#define ADAPTIVE_STRETCH 0
#if ADAPTIVE_STRETCH
  #define IF_ADAPTIVE(x) x
#else
  #define IF_ADAPTIVE(x)
#endif

void WidgetListH::onComputeSize () {
  WidgetListH* self = this;
  self->minSize = Vec2f_Create(0, 0);
  IF_ADAPTIVE( self->stretch = Vec2f_Create(0, 0); )
  for (Widget* child = self->head; child; child = child->next) {
    child->onComputeSize();
    self->minSize.y = Max(self->minSize.y, child->minSize.y);
    self->minSize.x += child->minSize.x;
    IF_ADAPTIVE(
      self->stretch.y = Max(self->stretch.y, child->stretch.y);
      self->stretch.x += child->stretch.x;)
    if (child != self->head) {
      self->minSize.x += self->spacing;
      IF_ADAPTIVE( self->stretch.x += self->spacing; )
    }
  }
}

void WidgetListV::onComputeSize () {
  WidgetListV* self = this;
  self->minSize = Vec2f_Create(0, 0);
  IF_ADAPTIVE( self->stretch = Vec2f_Create(0, 0); )
  for (Widget* child = self->head; child; child = child->next) {
    child->onComputeSize();
    self->minSize.x = Max(self->minSize.x, child->minSize.x);
    self->minSize.y += child->minSize.y;
    IF_ADAPTIVE(
      self->stretch.x = Max(self->stretch.x, child->stretch.x);
      self->stretch.y += child->stretch.y;)
    if (child != self->head) {
      self->minSize.y += self->spacing;
      IF_ADAPTIVE( self->stretch.y += self->spacing; )
    }
  }
}

void WidgetListH::onLayout () {
  WidgetListH* self = this;
  float totalStretch = 0.0f;
  float totalSize = 0.0f;

  /* Min size */ {
    for (Widget* child = self->head; child; child = child->next) {
      child->size.y = self->size.y;
      child->size.x = child->minSize.x;
      totalStretch += child->stretch.x;
      totalSize += child->size.x;
    }
  }

  /* Distribute extra space according to stretch. */ {
    float extra = self->size.x - self->minSize.x;
    if (totalStretch > 0.0f) {
      extra /= totalStretch;
      for (Widget* child = self->head; child; child = child->next)
        child->size.x += extra * child->stretch.x;
    }
  }

  /* Final position & recursive layout. */ {
    Vec2f pos = self->pos;
    for (Widget* child = self->head; child; child = child->next) {
      child->pos = pos;
      child->onLayout();
      pos.x += child->size.x + self->spacing;
    }
  }
}

void WidgetListV::onLayout () {
  WidgetListV* self = this;
  float totalStretch = 0.0f;
  float totalSize = 0.0f;

  /* Min size */ {
    for (Widget* child = self->head; child; child = child->next) {
      child->size.x = self->size.x;
      child->size.y = child->minSize.y;
      totalStretch += child->stretch.y;
      totalSize += child->size.y;
    }
  }

  /* Distribute extra space according to stretch. */ {
    float extra = self->size.y - self->minSize.y;
    if (totalStretch > 0.0f) {
      extra /= totalStretch;
      for (Widget* child = self->head; child; child = child->next)
        child->size.y += extra * child->stretch.y;
    }
  }

  /* Final position & recursive layout. */ {
    Vec2f pos = self->pos;
    for (Widget* child = self->head; child; child = child->next) {
      child->pos = pos;
      child->onLayout();
      pos.y += child->size.y + self->spacing;
    }
  }
}

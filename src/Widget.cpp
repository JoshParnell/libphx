#include "PhxMemory.h"
#include "Widget.h"
#include "WidgetDef.h"

#define STATUS_DELETED  0x00000001U
#define STATUS_LOCKED   0x00000002U

#define STATUS_GET(x, s) ((x)->status & (s))
#define STATUS_SET(x, s) ((x)->status |= (s))
#define STATUS_UNSET(x, s) ((x)->status &= ~(s))

static void Widget_FreeImpl (Widget* self) {
  Widget* child = self->head;
  while (child) {
    Widget* next = child->next;
    Widget_FreeImpl(child);
    child = next;
  }
  delete self;
}

void Widget_Free (Widget* self) {
  Widget* parent = self->parent;
  if (parent && STATUS_GET(parent, STATUS_LOCKED)) {
    STATUS_SET(self, STATUS_DELETED);
    return;
  }

  if (parent) {
    (self->next ? self->next->prev : parent->tail) = self->prev;
    (self->prev ? self->prev->next : parent->head) = self->next;
  }

  Widget_FreeImpl(self);
}

void Widget_Draw (Widget* self) {
  self->onDrawPre();
  STATUS_SET(self, STATUS_LOCKED);
  for (Widget* child = self->head; child; child = child->next)
    if (!STATUS_GET(child, STATUS_DELETED))
      Widget_Draw(child);
  STATUS_UNSET(self, STATUS_LOCKED);
  self->onDrawPost();
}

void Widget_Layout (Widget* self) {
  STATUS_SET(self, STATUS_LOCKED);
  self->onComputeSize();
  self->onLayout();
  STATUS_UNSET(self, STATUS_LOCKED);
}

void Widget_Update (Widget* self, float dt) {
  self->onUpdatePre(dt);

  STATUS_SET(self, STATUS_LOCKED);
  /* Update children. */ {
    for (Widget* child = self->head; child; child = child->next) {
      if (!STATUS_GET(child, STATUS_DELETED))
        Widget_Update(child, dt);
    }
  }
  STATUS_UNSET(self, STATUS_LOCKED);

  /* Remove deleted children. */ {
    Widget* child = self->head;
    while (child) {
      Widget* next = child->next;
      if (STATUS_GET(child, STATUS_DELETED)) {
        (child->next ? child->next->prev : self->tail) = child->prev;
        (child->prev ? child->prev->next : self->head) = child->next;
        Widget_FreeImpl(child);
      }
      child = next;
    }
  }

  self->onUpdatePost(dt);
}

void Widget_Add (Widget* parent, Widget* child) {
  if (child->parent)
    Fatal("Widget_Add: Attempting to add child that already has a parent");
  if (parent == child)
    Fatal("Widget_Add: Attempting to add a widget to itself");
  if (STATUS_GET(parent, STATUS_LOCKED))
    Fatal("Widget_Add: Attempting to add to a parent that is locked");

  child->parent = parent;

  if (parent->tail) {
    Widget* tail = parent->tail;
    tail->next = child;
    child->next = 0;
    child->prev = tail;
    parent->tail = child;
  } else {
    parent->head = child;
    parent->tail = child;
    child->next = 0;
    child->prev = 0;
  }
}

Widget* Widget_GetHead (Widget* self) {
  return self->head;
}

Widget* Widget_GetTail (Widget* self) {
  return self->tail;
}

Widget* Widget_GetParent (Widget* self) {
  return self->parent;
}

Widget* Widget_GetNext (Widget* self) {
  return self->next;
}

Widget* Widget_GetPrev (Widget* self) {
  return self->prev;
}

void Widget_SetMinSize (Widget* self, float sx, float sy) {
  self->minSize.x = sx;
  self->minSize.y = sy;
}

void Widget_SetPos (Widget* self, float x, float y) {
  self->pos.x = x;
  self->pos.y = y;
}

void Widget_SetSize (Widget* self, float sx, float sy) {
  self->size.x = sx;
  self->size.y = sy;
}

void Widget_SetStretch (Widget* self, float stretchX, float stretchY) {
  self->stretch.x = stretchX;
  self->stretch.y = stretchY;
}

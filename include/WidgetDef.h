#ifndef PHX_WidgetDef
#define PHX_WidgetDef
#define __FFI_IGNORE__

#include "Common.h"
#include "Vec2.h"

struct Widget {
  Widget* parent;
  Widget* head;
  Widget* tail;
  Widget* next;
  Widget* prev;
  Vec2f pos;
  Vec2f size;
  Vec2f minSize;
  Vec2f stretch;
  uint32 status;

  Widget () {
    parent = 0;
    head = 0;
    tail = 0;
    next = 0;
    prev = 0;
    pos.x = 0;
    pos.y = 0;
    size.x = 0;
    size.y = 0;
    minSize.x = 0;
    minSize.y = 0;
    stretch.x = 0;
    stretch.y = 0;
    status = 0;
  }

  virtual ~Widget () {}

  virtual void onDrawPre () {}
  virtual void onDrawPost () {}
  virtual void onComputeSize () {}
  virtual void onLayout () {}
  virtual void onUpdatePre (float dt) {}
  virtual void onUpdatePost (float dt) {}
};

#endif

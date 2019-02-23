#ifndef PHX_Widget
#define PHX_Widget

#include "Common.h"

PHX_API void  Widget_Free         (Widget*);

PHX_API void  Widget_Draw         (Widget*);
PHX_API void  Widget_Layout       (Widget*);
PHX_API void  Widget_Update       (Widget*, float dt);

PHX_API void  Widget_Add          (Widget* parent, Widget* child);
PHX_API void  Widget_SetMinSize   (Widget*, float sx, float sy);
PHX_API void  Widget_SetPos       (Widget*, float x, float y);
PHX_API void  Widget_SetSize      (Widget*, float sx, float sy);
PHX_API void  Widget_SetStretch   (Widget*, float stretchX, float stretchY);

PHX_API Widget*   Widget_GetHead      (Widget*);
PHX_API Widget*   Widget_GetTail      (Widget*);
PHX_API Widget*   Widget_GetParent    (Widget*);
PHX_API Widget*   Widget_GetNext      (Widget*);
PHX_API Widget*   Widget_GetPrev      (Widget*);

PHX_API Widget*   Widget_CreateBox    (float r, float g, float b, float a);
PHX_API Widget*   Widget_CreateListH  (float spacing);
PHX_API Widget*   Widget_CreateListV  (float spacing);

#endif

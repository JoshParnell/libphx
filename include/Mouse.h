#ifndef PHX_Mouse
#define PHX_Mouse

/* TODO DEPRECATED : Remove this API */
#define __FFI_IGNORE__

#include "Common.h"

PHX_API void   Mouse_GetDelta          (Vec2i* out);
PHX_API double Mouse_GetIdleTime       ();
PHX_API void   Mouse_GetPosition       (Vec2i* out);
PHX_API void   Mouse_GetPositionGlobal (Vec2i* out);
PHX_API int    Mouse_GetScroll         ();
PHX_API void   Mouse_SetPosition       (int, int);
PHX_API void   Mouse_SetVisible        (bool);

PHX_API bool   Mouse_Down              (MouseButton);
PHX_API bool   Mouse_Pressed           (MouseButton);
PHX_API bool   Mouse_Released          (MouseButton);

PRIVATE void Mouse_Init      ();
PRIVATE void Mouse_Free      ();
PRIVATE void Mouse_SetScroll (int amount);
PRIVATE void Mouse_Update    ();

#endif

#ifndef PHX_Keyboard
#define PHX_Keyboard

/* TODO DEPRECATED : Remove this API */
#define __FFI_IGNORE__

#include "Common.h"

PHX_API bool    Keyboard_Down         (Key);
PHX_API bool    Keyboard_Pressed      (Key);
PHX_API bool    Keyboard_Released     (Key);
PHX_API double  Keyboard_GetIdleTime  ();

PHX_API bool    KeyMod_Alt            ();
PHX_API bool    KeyMod_Ctrl           ();
PHX_API bool    KeyMod_Shift          ();

PRIVATE void Keyboard_Init       ();
PRIVATE void Keyboard_Free       ();
PRIVATE void Keyboard_UpdatePre  ();
PRIVATE void Keyboard_UpdatePost ();

#endif

#ifndef PHX_Joystick
#define PHX_Joystick

/* TODO DEPRECATED : Remove this API */
#define __FFI_IGNORE__

#include "Common.h"

PHX_API  int        Joystick_GetCount        ();
PHX_API  Joystick*  Joystick_Open            (int index);
PHX_API  void       Joystick_Close           (Joystick*);

PHX_API  cstr       Joystick_GetGUID         (Joystick*);
PHX_API  cstr       Joystick_GetGUIDByIndex  (int index);
PHX_API  cstr       Joystick_GetName         (Joystick*);
PHX_API  cstr       Joystick_GetNameByIndex  (int index);
PHX_API  int        Joystick_GetAxisCount    (Joystick*);
PHX_API  int        Joystick_GetBallCount    (Joystick*);
PHX_API  int        Joystick_GetButtonCount  (Joystick*);
PHX_API  int        Joystick_GetHatCount     (Joystick*);
PHX_API  double     Joystick_GetIdleTime     (Joystick*);

PHX_API  double     Joystick_GetAxis         (Joystick*, int index);
PHX_API  bool       Joystick_GetAxisAlive    (Joystick*, int index);
PHX_API  double     Joystick_GetAxisDelta    (Joystick*, int index);
PHX_API  HatDir     Joystick_GetHat          (Joystick*, int index);

PHX_API  bool       Joystick_ButtonDown      (Joystick*, int index);
PHX_API  bool       Joystick_ButtonPressed   (Joystick*, int index);
PHX_API  bool       Joystick_ButtonReleased  (Joystick*, int index);

/* --- Private API ---------------------------------------------------------- */

PRIVATE void        Joystick_Update          ();

#endif

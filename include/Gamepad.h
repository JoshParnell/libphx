#ifndef PHX_Gamepad
#define PHX_Gamepad

/* TODO DEPRECATED : Remove this API */
#define __FFI_IGNORE__

#include "Common.h"

PHX_API  bool      Gamepad_CanOpen            (int index);
PHX_API  Gamepad*  Gamepad_Open               (int index);
PHX_API  void      Gamepad_Close              (Gamepad*);

PHX_API  int       Gamepad_AddMappings        (cstr file);

PHX_API  double    Gamepad_GetAxis            (Gamepad*, GamepadAxis);
PHX_API  double    Gamepad_GetAxisDelta       (Gamepad*, GamepadAxis);
PHX_API  bool      Gamepad_GetButton          (Gamepad*, GamepadButton);
PHX_API  double    Gamepad_GetButtonPressed   (Gamepad*, GamepadButton);
PHX_API  double    Gamepad_GetButtonReleased  (Gamepad*, GamepadButton);
PHX_API  double    Gamepad_GetIdleTime        (Gamepad*);
PHX_API  int       Gamepad_GetID              (Gamepad*);
PHX_API  cstr      Gamepad_GetName            (Gamepad*);
PHX_API  bool      Gamepad_IsConnected        (Gamepad*);
PHX_API  void      Gamepad_SetDeadzone        (Gamepad*, GamepadAxis, double);

/* --- Private API ---------------------------------------------------------- */

PRIVATE void       Gamepad_Update             ();

#endif

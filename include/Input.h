#ifndef PHX_Input
#define PHX_Input

#include "Common.h"

/* --- Input -------------------------------------------------------------------
 *
 *   The low level engine API for reading input from the system. Input can be
 *   read using two paradigms: direct queries or through an event loop that is
 *   pumped by the application.
 *   (There is also a high level API, InputBindings)
 *
 *   The Button enum defines every possible input the engine supports. Notice
 *   that both button (e.g. Button_Keyboard_Space) and axis
 *   (Button_Gamepad_LTrigger) inputs are present in the enum. This is because
 *   every input, no matter it's 'type' or source can be read as a button with
 *   pressed/down/released state and as a axis with a [-1, 1] value.
 *
 *   This API will *always* provide button pressed and released states/events in
 *   perfect pairs. It is not possible to see 2 presses in a row, 2 released
 *   events in a row, a press without a release, or a release without a press.
 *
 *   Note that mouse movement is not translated to button presses/releases.
 *
 *   When a device is disconnected (currently only gamepads) all input will be
 *   set to zero (and yield the appropriate release states/events). Remapping a
 *   gamepad will yield the appropriate pressed and released states/events.
 *
 *
 *   Direct Queries
 *   --------------
 *   Input_GetPressed       : Return information about the button on the
 *   Input_GetDown            matching device with id 0.
 *   Input_GetReleased
 *   Input_GetValue
 *
 *   Input_GetChanged       : Check if a value has changed
 *   Input_GetDelta         : Returns the change in value since the last frame.
 *
 *   Input_GetIdleTime      : Returns time since the last input of any sort.
 *
 *   Input_GetActiveDevice* : Return information about a specific Device. These
 *   Input_Device*            also provide additional functionality for specific
 *   Input_Mouse*             DeviceTypes.
 *   Input_Keyboard*
 *   Input_Gamepad*
 *
 *
 *   Event Loop
 *   ----------
 *   Input_GetEventCount    : Unread events will be thrown away at the beginning
 *   Input_GetNextEvent       of the next update so they don't pile up when the
 *                            event loop isn't being utilized.
 *
 * -------------------------------------------------------------------------- */

PHX_API void        Input_LoadGamepadDatabase      (cstr filepath);

/* --- Direct Query API ----------------------------------------------------- */

PHX_API bool        Input_GetPressed               (Button);
PHX_API bool        Input_GetDown                  (Button);
PHX_API bool        Input_GetReleased              (Button);
PHX_API float       Input_GetValue                 (Button);
//PHX_API bool        Input_GetChanged               (Button);
//PHX_API float       Input_GetDelta                 (Button);
PHX_API float       Input_GetIdleTime              ();

PHX_API void        Input_GetActiveDevice          (Device*);
PHX_API DeviceType  Input_GetActiveDeviceType      ();
PHX_API uint32      Input_GetActiveDeviceID        ();
PHX_API float       Input_GetActiveDeviceIdleTime  ();

PHX_API bool        Input_GetDevicePressed         (Device*, Button);
PHX_API bool        Input_GetDeviceDown            (Device*, Button);
PHX_API bool        Input_GetDeviceReleased        (Device*, Button);
PHX_API float       Input_GetDeviceValue           (Device*, Button);
PHX_API float       Input_GetDeviceIdleTime        (Device*);

PHX_API void        Input_GetMouseDelta            (Vec2i*);
PHX_API float       Input_GetMouseIdleTime         ();
PHX_API void        Input_GetMousePosition         (Vec2i*);
PHX_API void        Input_GetMouseScroll           (Vec2i*);
PHX_API void        Input_SetMousePosition         (Vec2i*);
PHX_API void        Input_SetMouseScroll           (Vec2i*);
PHX_API void        Input_SetMouseVisible          (bool);
PHX_API void        Input_SetMouseVisibleAuto      ();

PHX_API float       Input_GetKeyboardIdleTime      ();
PHX_API bool        Input_GetKeyboardMod           (Modifier);
PHX_API bool        Input_GetKeyboardAlt           ();
PHX_API bool        Input_GetKeyboardCtrl          ();
PHX_API bool        Input_GetKeyboardShift         ();

PHX_API float       Input_GetGamepadIdleTime       (uint32 id);
PHX_API bool        Input_GetGamepadPressed        (uint32 id, Button);
PHX_API bool        Input_GetGamepadDown           (uint32 id, Button);
PHX_API bool        Input_GetGamepadReleased       (uint32 id, Button);
PHX_API float       Input_GetGamepadValue          (uint32 id, Button);
//PHX_API cstr        Input_GetGamepadName           (uint32 id);
//PHX_API bool        Input_IsGamepadConnected       (uint32 id);

/* --- Event Loop API ------------------------------------------------------- */

PHX_API int32       Input_GetEventCount            ();
PHX_API bool        Input_GetNextEvent             (InputEvent*);

/* --- Private API ---------------------------------------------------------- */

PRIVATE void        Input_Init                     ();
PRIVATE void        Input_Free                     ();
PRIVATE void        Input_Update                   ();

#endif

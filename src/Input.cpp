#include "ArrayList.h"
#include "Button.h"
#include "Device.h"
#include "DeviceType.h"
#include "Input.h"
#include "InputEvent.h"
#include "Modifier.h"
#include "PhxFloat.h"
#include "PhxMath.h"
#include "Profiler.h"
#include "Resource.h"
#include "SDL.h"
#include "State.h"
#include "Vec2.h"

/* --- Private API / Primary Functionality ---------------------------------- */

static const float Threshold_Pressed  = 0.5f;
static const float Threshold_Released = 0.4f;

struct DeviceState {
  int32  transitions[SDL_NUM_SCANCODES];
  bool   buttons[SDL_NUM_SCANCODES];
  float  axes[SDL_NUM_SCANCODES];
  uint32 lastEventTimestamp;
  bool   isConnected;
};

struct DeviceList {
  ArrayList(DeviceState, devices);
};

struct Input {
  Device                activeDevice;
  uint32                lastTimestamp;
  uint32                lastEventTimestamp;
  Vec2i                 lastMousePosition;
  bool                  autoHideMouse;
  DeviceList            deviceLists[DeviceType_COUNT];
  ArrayList(InputEvent, events);
  ArrayList(InputEvent, downButtons);
  ArrayList(InputEvent, autoRelease);
  ArrayList(InputEvent, injectedEvents);
} static self = { 0 };

inline static DeviceState* Input_EnsureDeviceState (Device device) {
  DeviceList* deviceList = &self.deviceLists[device.type];
  while ((uint32) ArrayList_GetSize(deviceList->devices) <= device.id) {
    DeviceState deviceState = {};
    ArrayList_Append(deviceList->devices, deviceState);
  }
  return ArrayList_GetPtr(deviceList->devices, device.id);
}

inline static DeviceState* Input_GetDeviceState (Device device) {
  DeviceList* deviceList = &self.deviceLists[device.type];
  return ArrayList_GetPtr(deviceList->devices, device.id);
}

inline static void Input_SetActiveDevice (Device device) {
  self.activeDevice = device;
  if (self.autoHideMouse)
    SDL_ShowCursor(device.type == DeviceType_Mouse ? SDL_ENABLE : SDL_DISABLE);
}

inline static bool Input_GetDeviceExists (Device device) {
  DeviceList* deviceList = &self.deviceLists[device.type];
  if (device.id < (uint32) ArrayList_GetSize(deviceList->devices)) {
    DeviceState* deviceState = ArrayList_GetPtr(deviceList->devices, device.id);
    return deviceState->isConnected;
  }
  return false;
}

inline static bool Input_GetDevicePressedImpl (Device device, Button button) {
  DeviceState* deviceState = Input_GetDeviceState(device);
  return deviceState->buttons[button]
    ? deviceState->transitions[button] > 0
    : deviceState->transitions[button] > 1;
}

inline static bool Input_GetDeviceDownImpl (Device device, Button button) {
  DeviceState* deviceState = Input_GetDeviceState(device);
  return deviceState->buttons[button] || deviceState->transitions[button] > 0;
}

inline static bool Input_GetDeviceReleasedImpl (Device device, Button button) {
  DeviceState* deviceState = Input_GetDeviceState(device);
  return deviceState->buttons[button]
    ? deviceState->transitions[button] > 1
    : deviceState->transitions[button] > 0;
}

inline static float Input_GetDeviceValueImpl (Device device, Button button) {
  DeviceState* deviceState = Input_GetDeviceState(device);
  return deviceState->axes[button];
}

inline static float Input_GetDeviceIdleTimeImpl (Device device) {
  DeviceState* deviceState = Input_GetDeviceState(device);
  return (self.lastTimestamp - deviceState->lastEventTimestamp) / 1000.0f;
}

inline static State Input_DetermineButtonState (InputEvent event) {
  State buttonState = State_Null;

  DeviceState* deviceState = Input_GetDeviceState(event.device);
  bool down = deviceState->buttons[event.button];
  if (!down && event.value > Threshold_Pressed)
    buttonState |= State_Pressed | State_Down;

  if (down && event.value < Threshold_Released)
    buttonState |= State_Released;

  return buttonState;
}

inline static void Input_AppendEvent (InputEvent event) {
  self.lastTimestamp      = event.timestamp;
  self.lastEventTimestamp = event.timestamp;
  ArrayList_Append(self.events, event);
};

inline static void Input_InjectEvent (InputEvent event) {
  self.lastTimestamp      = event.timestamp;
  self.lastEventTimestamp = event.timestamp;
  ArrayList_Append(self.injectedEvents, event);
};

inline static void Input_SetButton (InputEvent event) {
  Assert(event.button != 0);

  DeviceState* deviceState = Input_GetDeviceState(event.device);
  deviceState->axes[event.button] = event.value;

  bool down = deviceState->buttons[event.button];
  if (!down && HAS_FLAG(event.state, State_Pressed)) {
    deviceState->transitions[event.button]++;
    deviceState->buttons[event.button] = true;

    ArrayList_Append(self.downButtons, event);

    if (event.device.type != DeviceType_Null)
      Input_SetActiveDevice(event.device);
  }

  if (down && HAS_FLAG(event.state, State_Released)) {
    deviceState->transitions[event.button]++;
    deviceState->buttons[event.button] = false;

    ArrayList_ForEachIReverse(self.downButtons, i) {
      if (ArrayList_Get(self.downButtons, i).button == event.button) {
        ArrayList_RemoveAt(self.downButtons, i);
      }
    }
  }

  if (Button_IsAutoRelease(event.button))
    ArrayList_Append(self.autoRelease, event);
}

void Input_Init () {
  /* NOTE : This is a workaround for what appears to be a bug in SDL. Without
   *        this the first click after a window loses and regains focus will not
   *        send events. This still leaves us with not-completely-consistent
   *        behavior. Since the mouse state doesn't get screwed up when using
   *        the keyboard, the first click after alt tabbing will send mouse
   *        down, but only the second click after clicking off the window will
   *        send mouse down. We could further work around this by only only
   *        setting the hint when clicking off the window, but eh, more
   *        complexity for a much less irritating quirk. This bug has been
   *        reported.
   *        https://bugzilla.libsdl.org/show_bug.cgi?id=4165 */
  SDL_bool result = SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
  if (result != SDL_TRUE)
    Warn("Input_Init: SDL_SetHint failed");

  for (int iDev = 0; iDev < DeviceType_COUNT; iDev++) {
    Device device = { iDev, 0 };
    DeviceState* deviceState = Input_EnsureDeviceState(device);
    deviceState->isConnected = iDev != DeviceType_Gamepad;
  }

  ArrayList_Reserve(self.events,         16);
  ArrayList_Reserve(self.downButtons,    16);
  ArrayList_Reserve(self.autoRelease,    16);
  ArrayList_Reserve(self.injectedEvents, 16);

  Device device = { DeviceType_Mouse, 0 };
  Input_SetActiveDevice(device);
}

void Input_Free () {
  for (int iDev = 0; iDev < DeviceType_COUNT; iDev++)
    ArrayList_Free(self.deviceLists[iDev].devices);

  ArrayList_Free(self.events);
  ArrayList_Free(self.downButtons);
  ArrayList_Free(self.autoRelease);
  ArrayList_Free(self.injectedEvents);
}

void Input_Update () {
  FRAME_BEGIN;


  { /* Update Stale Data */
    self.lastTimestamp       = (uint32) SDL_GetTicks();
    self.lastMousePosition.x = (int) Input_GetValue(Button_Mouse_X);
    self.lastMousePosition.y = (int) Input_GetValue(Button_Mouse_Y);

    for (int iDev = 0; iDev < DeviceType_COUNT; iDev++) {
      DeviceList* deviceList = &self.deviceLists[iDev];
      ArrayList_ForEach(deviceList->devices, DeviceState, deviceState) {
        MemSet(deviceState->transitions, 0, sizeof(deviceState->transitions));
      }
    }

    ArrayList_Clear(self.events);
    ArrayList_ForEach(self.injectedEvents, InputEvent, event) {
      Input_AppendEvent(*event);
    }
    ArrayList_Clear(self.injectedEvents);
  }


  { /* Process Down Buttons */
    ArrayList_ForEach(self.autoRelease, InputEvent, down) {
      DeviceState* deviceState = Input_GetDeviceState(down->device);

      if (deviceState->axes[down->button] != 0.0f) {
        down->value     = 0.0f;
        down->state     = State_Changed | Input_DetermineButtonState(*down);
        down->timestamp = (uint32) SDL_GetTicks();
        Input_SetButton(*down);
        Input_AppendEvent(*down);
      }
    }

    ArrayList_ForEach(self.downButtons, InputEvent, down) {
      DeviceState* deviceState = Input_GetDeviceState(down->device);

      down->value     = deviceState->axes[down->button];
      down->state     = State_Down;
      down->timestamp = (uint32) SDL_GetTicks();
      Input_AppendEvent(*down);
    }
  }


  { /* Process New Input */
    SDL_Event sdl;
    while (SDL_PollEvent(&sdl) != 0) {
      InputEvent event = {};
      event.timestamp = sdl.common.timestamp;

      switch(sdl.type) {


        /* DeviceType_Keyboard */
        case SDL_KEYDOWN: {
          if (sdl.key.repeat) continue;
          Device device = { DeviceType_Keyboard, 0 };

          event.device = device;
          event.button = Button_FromSDLScancode(sdl.key.keysym.scancode);
          event.value  = 1.0f;
          event.state  = State_Changed | State_Pressed | State_Down;
          if (event.button == Button_Null) continue;
          Input_SetButton(event);
          Input_AppendEvent(event);
        }
        break;

        case SDL_KEYUP: {
          Device device = { DeviceType_Keyboard, 0 };

          event.device = device;
          event.button = Button_FromSDLScancode(sdl.key.keysym.scancode);
          event.value  = 0.0f;
          event.state  = State_Changed | State_Released;
          if (event.button == Button_Null) continue;
          Input_SetButton(event);
          Input_AppendEvent(event);
        }
        break;



        /* DeviceType_Mouse */
        case SDL_MOUSEBUTTONDOWN: {
          Device device = { DeviceType_Mouse, sdl.button.which };

          event.device = device;
          event.button = Button_FromSDLMouseButton(sdl.button.button);
          event.value  = 1.0f;
          event.state  = State_Changed | State_Pressed | State_Down;
          Input_EnsureDeviceState(event.device);
          Input_SetButton(event);
          Input_AppendEvent(event);
        }
        break;

        case SDL_MOUSEBUTTONUP: {
          Device device = { DeviceType_Mouse, sdl.button.which };

          event.device = device;
          event.button = Button_FromSDLMouseButton(sdl.button.button);
          event.value  = 0.0f;
          event.state  = State_Changed | State_Released;
          Input_EnsureDeviceState(event.device);
          Input_SetButton(event);
          Input_AppendEvent(event);
        }
        break;

        case SDL_MOUSEMOTION: {
          /* NOTE : Mouse motion never causes pressed/down/released events. */
          Device device = { DeviceType_Mouse, sdl.motion.which };
          DeviceState* deviceState = Input_EnsureDeviceState(device);

          event.device = device;
          event.button = Button_Mouse_X;
          event.value  = (float) sdl.motion.x;
          event.state  = State_Changed;
          if (event.value != deviceState->axes[event.button]) {
            deviceState->axes[event.button] = event.value;
            Input_SetActiveDevice(event.device);
            Input_AppendEvent(event);
          }

          event.device = device;
          event.button = Button_Mouse_Y;
          event.value  = (float) sdl.motion.y;
          event.state  = State_Changed;
          if (event.value != deviceState->axes[event.button]) {
            deviceState->axes[event.button] = event.value;
            Input_SetActiveDevice(event.device);
            Input_AppendEvent(event);
          }
        }
        break;

        case SDL_MOUSEWHEEL: {
          Device device = { DeviceType_Mouse, sdl.wheel.which };
          Input_EnsureDeviceState(device);

          event.device = device;
          event.button = Button_Mouse_ScrollX;
          event.value  = (float) sdl.wheel.x;
          event.state  = State_Changed | Input_DetermineButtonState(event);
          if (event.value != Input_GetDeviceValueImpl(event.device, event.button)) {
            Input_SetButton(event);
            Input_AppendEvent(event);
          }

          event.device = device;
          event.button = Button_Mouse_ScrollY;
          event.value  = (float) sdl.wheel.y;
          event.state  = State_Changed | Input_DetermineButtonState(event);
          if (event.value != Input_GetDeviceValueImpl(event.device, event.button)) {
            Input_SetButton(event);
            Input_AppendEvent(event);
          }
        }
        break;


        /* DeviceType_Gamepad */
        case SDL_CONTROLLERBUTTONDOWN: {
          Device device = { DeviceType_Gamepad, (uint32) sdl.cbutton.which };

          event.device = device;
          event.button = Button_FromSDLControllerButton((SDL_GameControllerButton) sdl.cbutton.button);
          event.value  = 1.0f;
          event.state  = State_Changed | State_Pressed | State_Down;
          Input_SetButton(event);
          Input_AppendEvent(event);
        }
        break;

        case SDL_CONTROLLERBUTTONUP: {
          Device device = { DeviceType_Gamepad, (uint32) sdl.cbutton.which };

          event.device = device;
          event.button = Button_FromSDLControllerButton((SDL_GameControllerButton) sdl.cbutton.button);
          event.value  = 0.0f;
          event.state  = State_Changed | State_Released;
          Input_SetButton(event);
          Input_AppendEvent(event);
        }
        break;

        case SDL_CONTROLLERAXISMOTION: {
          Device device = { DeviceType_Gamepad, (uint32) sdl.caxis.which };
          float value = Clamp((float) sdl.caxis.value / 32767.0f, -1.0f, 1.0f);
          SDL_GameControllerAxis axis = (SDL_GameControllerAxis) sdl.caxis.axis;
          if (axis == SDL_CONTROLLER_AXIS_LEFTY || axis == SDL_CONTROLLER_AXIS_RIGHTY)
            value = -value;

          event.device = device;
          event.button = Button_FromSDLControllerAxis(axis);
          event.value  = value;
          event.state  = State_Changed | Input_DetermineButtonState(event);
          Input_SetButton(event);
          Input_AppendEvent(event);
        }
        break;

        case SDL_CONTROLLERDEVICEADDED: {
          if (SDL_IsGameController(sdl.cdevice.which) == SDL_TRUE) {
            SDL_GameController* sdlController = SDL_GameControllerOpen(sdl.cdevice.which);
            if (sdlController == 0) {
              Warn("Input_Update: SDL_GameControllerOpen failed");
            }
            else {
              SDL_Joystick* sdlJoystick = SDL_GameControllerGetJoystick(sdlController);
              uint32        id          = (uint32) SDL_JoystickInstanceID(sdlJoystick);
              Device        device      = { DeviceType_Gamepad, id };
              DeviceState*  deviceState = Input_EnsureDeviceState(device);
              deviceState->isConnected  = true;
            }
          }
        }
        break;

        case SDL_CONTROLLERDEVICEREMOVED: {
          /* NOTE : SDL already sends events to zero out all game controller
           *        input so there's no need to do it manually. */

          Device        device      = { DeviceType_Gamepad, (uint32) sdl.cdevice.which };
          DeviceState*  deviceState = Input_GetDeviceState(device);
          deviceState->isConnected  = false;

          SDL_GameController* sdlController = SDL_GameControllerFromInstanceID(sdl.cdevice.which);
          if (sdlController)
            SDL_GameControllerClose(sdlController);
        }
        break;

        /* TODO : Maybe we should release all input then re-set it? */
        case SDL_CONTROLLERDEVICEREMAPPED: {
          SDL_GameController* sdlController = SDL_GameControllerFromInstanceID(sdl.cdevice.which);
          Device              device        = { DeviceType_Gamepad, (uint32) sdl.cdevice.which };
          DeviceState*        deviceState   = Input_GetDeviceState(device);

          for (int32 iBtn = Button_Gamepad_Button_First; iBtn <= Button_Gamepad_Button_Last; iBtn++) {
            float value = (float) SDL_GameControllerGetButton(sdlController, Button_ToSDLControllerButton(iBtn));
            if (value != deviceState->axes[iBtn]) {
              event.device = device;
              event.button = iBtn;
              event.value  = value;
              event.state  = State_Changed | Input_DetermineButtonState(event);
              Input_SetButton(event);
              Input_AppendEvent(event);
            }
          }

          for (int32 iAxis = Button_Gamepad_Axis_First; iAxis <= Button_Gamepad_Axis_Last; iAxis++) {
            float value = (float) SDL_GameControllerGetAxis(sdlController, Button_ToSDLControllerAxis(iAxis));
            value = Clamp(value / 32767.0f, -1.0f, 1.0f);
            if (iAxis == Button_Gamepad_LStickY || iAxis == Button_Gamepad_RStickY)
              value = -value;

            if (value != deviceState->axes[iAxis]) {
              event.device = device;
              event.button = iAxis;
              event.value  = value;
              event.state  = State_Changed | Input_DetermineButtonState(event);
              Input_SetButton(event);
              Input_AppendEvent(event);
            }
          }
        }
        break;


        /* DeviceType_Null */
        case SDL_QUIT: {
          Device device = { DeviceType_Null, 0 };

          event.device = device;
          event.button = Button_System_Exit;
          event.value  = 0.0f;
          event.state  = State_Changed | State_Pressed | State_Down;
          Input_SetButton(event);
          Input_AppendEvent(event);
        }
        break;

        case SDL_WINDOWEVENT: {
          if (sdl.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
            SDL_CaptureMouse(SDL_TRUE);

          /* TODO : Test button release on focus loss */
          if (sdl.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            SDL_CaptureMouse(SDL_FALSE);

            /* OPTIMIZE : Do this without incurring the cost of the search and
             *            removes in SetButton */
            ArrayList_ForEach(self.downButtons, InputEvent, down) {
              down->timestamp = sdl.common.timestamp;
              down->value     = 0.0f;
              down->state     = State_Changed | Input_DetermineButtonState(event);
              Input_SetButton(*down);
              Input_AppendEvent(*down);
            }
          }
        }
        break;
      }
    }
  }


  FRAME_END;
}

void Input_LoadGamepadDatabase (cstr name) {
  cstr path = Resource_GetPath(ResourceType_Other, name);
  int result = SDL_GameControllerAddMappingsFromFile(path);
  if (result == -1)
    Fatal("Input_Init: Failed to add gamepad mappings");
}

/* --- Direct Query API (Automatic Device) ---------------------------------- */

bool Input_GetPressed (Button button) {
  Device device = { Button_ToDeviceType(button), 0 };
  return Input_GetDevicePressedImpl(device, button);
}

bool Input_GetDown (Button button) {
  Device device = { Button_ToDeviceType(button), 0 };
  return Input_GetDeviceDownImpl(device, button);
}

bool Input_GetReleased (Button button) {
  Device device = { Button_ToDeviceType(button), 0 };
  return Input_GetDeviceReleasedImpl(device, button);
}

float Input_GetValue (Button button) {
  Device device = { Button_ToDeviceType(button), 0 };
  return Input_GetDeviceValueImpl(device, button);
}

float Input_GetIdleTime () {
  return (self.lastTimestamp - self.lastEventTimestamp) / 1000.0f;
}


/* --- Direct Query API (Active Device) ------------------------------------- */

void Input_GetActiveDevice (Device* device) {
  *device = self.activeDevice;
}

DeviceType Input_GetActiveDeviceType () {
  return self.activeDevice.type;
}

uint32 Input_GetActiveDeviceID () {
  return self.activeDevice.id;
}

float Input_GetActiveDeviceIdleTime () {
  return Input_GetDeviceIdleTimeImpl(self.activeDevice);
}


/* --- Direct Query API (Specified Device) ---------------------------------- */

bool Input_GetDevicePressed (Device* device, Button button) {
  if (!Input_GetDeviceExists(*device))
    return false;
  return Input_GetDevicePressedImpl(*device, button);
}

bool Input_GetDeviceDown (Device* device, Button button) {
  if (!Input_GetDeviceExists(*device))
    return false;
  return Input_GetDeviceDownImpl(*device, button);
}

bool Input_GetDeviceReleased (Device* device, Button button) {
  if (!Input_GetDeviceExists(*device))
    return false;
  return Input_GetDeviceReleasedImpl(*device, button);
}

float Input_GetDeviceValue (Device* device, Button button) {
  if (!Input_GetDeviceExists(*device))
    return 0.0f;
  return Input_GetDeviceValueImpl(*device, button);
}

float Input_GetDeviceIdleTime (Device* device) {
  if (!Input_GetDeviceExists(*device))
    return FLT_MAX;
  return Input_GetDeviceIdleTimeImpl(*device);
}


/* --- Direct Query API (Mouse Device) -------------------------------------- */

void Input_GetMouseDelta (Vec2i* delta) {
  delta->x = (int) Input_GetValue(Button_Mouse_X) - self.lastMousePosition.x;
  delta->y = (int) Input_GetValue(Button_Mouse_Y) - self.lastMousePosition.y;
}

float Input_GetMouseIdleTime () {
  Device device = { DeviceType_Mouse, 0 };
  return Input_GetDeviceIdleTimeImpl(device);
}

void Input_GetMousePosition (Vec2i* position) {
  Device device = { DeviceType_Mouse, 0 };
  position->x = (int) Input_GetDeviceValueImpl(device, Button_Mouse_X);
  position->y = (int) Input_GetDeviceValueImpl(device, Button_Mouse_Y);
}

void Input_GetMouseScroll (Vec2i* scroll) {
  Device device = { DeviceType_Mouse, 0 };
  scroll->x = (int) Input_GetDeviceValueImpl(device, Button_Mouse_ScrollX);
  scroll->y = (int) Input_GetDeviceValueImpl(device, Button_Mouse_ScrollY);
}

void Input_SetMousePosition (Vec2i* position) {
  SDL_WarpMouseInWindow(0, position->x, position->y);
}

void Input_SetMouseVisible (bool visible) {
  self.autoHideMouse = false;
  SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

void Input_SetMouseVisibleAuto () {
  self.autoHideMouse = true;
  Input_SetActiveDevice(self.activeDevice);
}

void Input_SetMouseScroll (Vec2i* scroll) {
  uint32 timestamp = (uint32) SDL_GetTicks();
  Device device    = { DeviceType_Mouse, 0 };

  InputEvent event = {};

  event.timestamp = timestamp;
  event.device    = device;
  event.button    = Button_Mouse_ScrollX;
  event.value     = (float) scroll->x;
  event.state     = State_Changed | Input_DetermineButtonState(event);
  if (event.value != Input_GetDeviceValueImpl(event.device, event.button))
    Input_InjectEvent(event);

  event.timestamp = timestamp;
  event.device    = device;
  event.button    = Button_Mouse_ScrollY;
  event.value     = (float) scroll->y;
  event.state     = State_Changed | Input_DetermineButtonState(event);
  if (event.value != Input_GetDeviceValueImpl(event.device, event.button))
    Input_InjectEvent(event);
}


/* --- Direct Query API (Keyboard Device) ----------------------------------- */

float Input_GetKeyboardIdleTime () {
  Device device = { DeviceType_Keyboard, 0 };
  return Input_GetDeviceIdleTimeImpl(device);
}

bool Input_GetKeyboardMod (Modifier modifier) {
  bool hasMod = true;
  if (HAS_FLAG(modifier, Modifier_Alt))   hasMod &= Input_GetKeyboardAlt();
  if (HAS_FLAG(modifier, Modifier_Ctrl))  hasMod &= Input_GetKeyboardCtrl();
  if (HAS_FLAG(modifier, Modifier_Shift)) hasMod &= Input_GetKeyboardShift();
  return hasMod;
}

bool Input_GetKeyboardAlt () {
  Device device = { DeviceType_Keyboard, 0 };
  DeviceState* deviceState = Input_GetDeviceState(device);
  return deviceState->buttons[Button_Keyboard_LAlt] || deviceState->buttons[Button_Keyboard_RAlt];
}

bool Input_GetKeyboardCtrl () {
  Device device = { DeviceType_Keyboard, 0 };
  DeviceState* deviceState = Input_GetDeviceState(device);
  return deviceState->buttons[Button_Keyboard_LCtrl] || deviceState->buttons[Button_Keyboard_RCtrl];
}

bool Input_GetKeyboardShift () {
  Device device = { DeviceType_Keyboard, 0 };
  DeviceState* deviceState = Input_GetDeviceState(device);
  return deviceState->buttons[Button_Keyboard_LShift] || deviceState->buttons[Button_Keyboard_RShift];
}


/* --- Direct Query API (Gamepad Device) ------------------------------------ */

float Input_GetGamepadIdleTime (uint32 id) {
  Device device = { DeviceType_Gamepad, id };
  if (!Input_GetDeviceExists(device))
    return FLT_MAX;
  return Input_GetDeviceIdleTimeImpl(device);
}

bool Input_GetGamepadPressed (uint32 id, Button button) {
  Device device = { DeviceType_Gamepad, id };
  if (!Input_GetDeviceExists(device))
    return false;
  return Input_GetDevicePressedImpl(device, button);
}

bool Input_GetGamepadDown (uint32 id, Button button) {
  Device device = { DeviceType_Gamepad, id };
  if (!Input_GetDeviceExists(device))
    return false;
  return Input_GetDeviceDownImpl(device, button);
}

bool Input_GetGamepadReleased (uint32 id, Button button) {
  Device device = { DeviceType_Gamepad, id };
  if (!Input_GetDeviceExists(device))
    return false;
  return Input_GetDeviceReleasedImpl(device, button);
}

float Input_GetGamepadValue (uint32 id, Button button) {
  Device device = { DeviceType_Gamepad, id };
  if (!Input_GetDeviceExists(device))
    return 0.0f;
  return Input_GetDeviceValueImpl(device, button);
}


/* --- Event Loop API ------------------------------------------------------- */

int32 Input_GetEventCount () {
  return ArrayList_GetSize(self.events);
}

bool Input_GetNextEvent (InputEvent* event) {
  if (ArrayList_GetSize(self.events) == 0) return false;
  FRAME_BEGIN;

  *event = ArrayList_Get(self.events, 0);
  ArrayList_RemoveAt(self.events, 0);

  FRAME_END;
  return true;
}

/* NOTE : Controller instance ID's are signed (sdl.cbutton/caxis/cdevice.which)
 *        while mouse instance ID's are unsigned (sdl.button/motion/wheel.which).
 *        This is a bit annoying since we can store both in Device.id. I've
 *        opted to store them as uint32's since they're used as array indices
 *        here (and presumably in SDL as well). */

/* NOTE : We keep a permanent DeviceState for every joystick instance id which
 *        is montonically increasing each time a joystick is connected. This is
 *        kind of leaky in a sense, but realistically seems like it's never
 *        going to be a problem. */

/* NOTE : SDL GameController Info
 *        SDL_CONTROLLERDEVICEADDED is always sent when a controller is connected
 *        SDL_CONTROLLERDEVICEREMOVED is only sent if the controller was opened
 *        SDL sends events to zero all controller input right before device removal
 *        SDL does not send any events to fix up input when a controller is remapped */

/* TODO : I changed instantaneous events such as Button_System_Exit to be
 *        pressed in the current frame then auto-released in the following
 *        frame. This probably makes sense for Button_Mouse_ScrollX/Y because we
 *        want GetValue queries to return the scroll value throughout the entire
 *        frame, but maybe makes less sense for Exit where the value is not
 *        terribly important and GetPressed will already return true. Supporting
 *        this, any random button that is pressed and released in the same frame
 *        already behave that way: value will be 0 and pressed will be true. */

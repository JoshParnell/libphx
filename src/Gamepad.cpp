#include "Gamepad.h"
#include "GamepadAxis.h"
#include "GamepadButton.h"
#include "LinkedList.h"
#include "PhxMath.h"
#include "PhxMemory.h"
#include "SDL.h"
#include "TimeStamp.h"

struct Gamepad {
  LinkedListCell(gamepadList, Gamepad);
  SDL_GameController* handle;
  TimeStamp lastActive;
  double axisState[GamepadAxis_SIZE];
  double axisLast[GamepadAxis_SIZE];
  double deadzone[GamepadAxis_SIZE];
  bool buttonState[GamepadButton_SIZE];
  bool buttonLast[GamepadButton_SIZE];
};

static LinkedList(gamepadList, Gamepad) = 0;

static void Gamepad_UpdateState (Gamepad* self) {
  TimeStamp now = TimeStamp_Get();
  for (GamepadAxis i = GamepadAxis_BEGIN; i <= GamepadAxis_END; ++i) {
    double state = Gamepad_GetAxis(self, i);
    if (self->axisState[i] != state)
      self->lastActive = now;
    self->axisLast[i] = self->axisState[i];
    self->axisState[i] = state;
  }

  for (GamepadButton i = GamepadButton_BEGIN; i <= GamepadButton_END; ++i) {
    bool state = Gamepad_GetButton(self, i);
    if (self->buttonState[i] != state)
      self->lastActive = now;
    self->buttonLast[i] = self->buttonState[i];
    self->buttonState[i] = state;
  }
}

bool Gamepad_CanOpen (int index) {
  return SDL_IsGameController(index) == SDL_TRUE;
}

Gamepad* Gamepad_Open (int index) {
  SDL_GameController* handle = SDL_GameControllerOpen(index);
  if (!handle)
    return 0;
  Gamepad* self = MemNewZero(Gamepad);
  self->handle = handle;
  self->lastActive = TimeStamp_Get();
  LinkedList_Insert(gamepadList, self);
  Gamepad_UpdateState(self);
  return self;
}

void Gamepad_Close (Gamepad* self) {
  LinkedList_Remove(gamepadList, self);
  SDL_GameControllerClose(self->handle);
  MemFree(self);
}

int Gamepad_AddMappings (cstr file) {
  return SDL_GameControllerAddMappingsFromFile(file);
}

double Gamepad_GetAxis (Gamepad* self, GamepadAxis axis) {
  double value = (double)SDL_GameControllerGetAxis(
    self->handle, (SDL_GameControllerAxis)axis) / 32767.0;
  double deadzone = self->deadzone[axis];
  if (value >  deadzone) return (value - deadzone) / (1.0 - deadzone);
  if (value < -deadzone) return (value + deadzone) / (1.0 - deadzone);
  return 0.0;
}

double Gamepad_GetAxisDelta (Gamepad* self, GamepadAxis axis) {
  return self->axisState[axis] - self->axisLast[axis];
}

bool Gamepad_GetButton (Gamepad* self, GamepadButton button) {
  return SDL_GameControllerGetButton(self->handle, (SDL_GameControllerButton)button) == 1;
}

double Gamepad_GetButtonPressed (Gamepad* self, GamepadButton button) {
  return (self->buttonState[button] && !self->buttonLast[button]) ? 1.0 : 0.0;
}

double Gamepad_GetButtonReleased (Gamepad* self, GamepadButton button) {
  return (!self->buttonState[button] && self->buttonLast[button]) ? 1.0 : 0.0;
}

double Gamepad_GetIdleTime (Gamepad* self) {
  return TimeStamp_GetElapsed(self->lastActive);
}

int Gamepad_GetID (Gamepad* self) {
  SDL_Joystick* joystick = SDL_GameControllerGetJoystick(self->handle);
  if (!joystick)
    return -1;
  return SDL_JoystickInstanceID(joystick);
}

cstr Gamepad_GetName (Gamepad* self) {
  return SDL_GameControllerName(self->handle);
}

bool Gamepad_IsConnected (Gamepad* self) {
  return SDL_GameControllerGetAttached(self->handle) == SDL_TRUE;
}

void Gamepad_SetDeadzone (Gamepad* self, GamepadAxis axis, double deadzone) {
  self->deadzone[axis] = deadzone;
}

void Gamepad_Update () {
  LinkedList_ForEach(gamepadList, Gamepad, self)
    Gamepad_UpdateState(self);
}

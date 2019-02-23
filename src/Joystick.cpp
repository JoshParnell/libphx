#include "Joystick.h"
#include "PhxMath.h"
#include "PhxMemory.h"
#include "PhxString.h"
#include "SDL.h"
#include "TimeStamp.h"

/* TODO : Use a linked-list instead of a freelist. This is lazy. */

struct Joystick {
  SDL_Joystick* handle;
  cstr guid;
  int axes;
  int balls;
  int buttons;
  int hats;
  bool* buttonStates;
  bool* axisAlive;
  double* axisStates;
  TimeStamp lastUsed;
};

static const int kMaxOpen = 64;
static int kOpen = 0;
static Joystick* freeList[kMaxOpen] = { 0 };

static char* ConvertGUID (SDL_JoystickGUID id) {
  static char buf[64];
  SDL_JoystickGetGUIDString(id, buf, sizeof(buf));
  return buf;
}

static void Joystick_UpdateSingle (Joystick* self) {
  bool changed = false;
  for (int i = 0; i < self->axes; ++i) {
    double state = Joystick_GetAxis(self, i);
    double delta = Abs(state - self->axisStates[i]);
    if (delta > 0.1) {
      changed = true;
      self->axisAlive[i] = true;
    }
    self->axisStates[i] = state;
  }

  for (int i = 0; i < self->buttons; ++i) {
    bool state = Joystick_ButtonDown(self, i);
    if (self->buttonStates[i] != state)
      changed = true;
    self->buttonStates[i] = state;
  }

  if (changed)
    self->lastUsed = TimeStamp_Get();
}

int Joystick_GetCount () {
  return SDL_NumJoysticks();
}

Joystick* Joystick_Open (int index) {
  Joystick* self = MemNew(Joystick);
  if (kOpen == kMaxOpen)
    Fatal("Cannot open any more gamepad connections.");

  for (int i = 0; i < kMaxOpen; ++i) {
    if (!freeList[i]) {
      freeList[i] = self;
      kOpen++;
      break;
    }
  }

  self->handle = SDL_JoystickOpen(index);
  self->guid = StrDup(ConvertGUID(SDL_JoystickGetGUID(self->handle)));
  self->axes = SDL_JoystickNumAxes(self->handle);
  self->balls = SDL_JoystickNumBalls(self->handle);
  self->buttons = SDL_JoystickNumButtons(self->handle);
  self->hats = SDL_JoystickNumHats(self->handle);
  self->buttonStates = MemNewArray(bool, self->buttons);
  self->axisAlive = MemNewArray(bool, self->axes);
  MemZero(self->axisAlive, sizeof(bool) * self->axes);
  self->axisStates = MemNewArray(double, self->axes);
  self->lastUsed = TimeStamp_Get();
  Joystick_UpdateSingle(self);
  return self;
}

void Joystick_Close (Joystick* self) {
  kOpen--;
  for (int i = 0; i < kMaxOpen; ++i) {
    if (freeList[i] == self) {
      freeList[i] = 0;
      break;
    }
  }

  SDL_JoystickClose(self->handle);
  MemFree(self->guid);
  MemFree(self->buttonStates);
  MemFree(self->axisStates);
  MemFree(self);
}

cstr Joystick_GetGUID (Joystick* self) {
  return self->guid;
}

cstr Joystick_GetGUIDByIndex (int index) {
  return (cstr)ConvertGUID(SDL_JoystickGetDeviceGUID(index));
}

cstr Joystick_GetName (Joystick* self) {
  return SDL_JoystickName(self->handle);
}

cstr Joystick_GetNameByIndex (int index) {
  return SDL_JoystickNameForIndex(index);
}

int Joystick_GetAxisCount (Joystick* self) {
  return self->axes;
}

int Joystick_GetBallCount (Joystick* self) {
  return self->balls;
}

int Joystick_GetButtonCount (Joystick* self) {
  return self->buttons;
}

int Joystick_GetHatCount (Joystick* self) {
  return self->hats;
}

double Joystick_GetIdleTime (Joystick* self) {
  return TimeStamp_GetElapsed(self->lastUsed);
}

double Joystick_GetAxis (Joystick* self, int index) {
  return SDL_JoystickGetAxis(self->handle, index) / 32768.0;
}

bool Joystick_GetAxisAlive (Joystick* self, int index) {
  return self->axisAlive[index];
}

double Joystick_GetAxisDelta (Joystick* self, int index) {
  return SDL_JoystickGetAxis(self->handle, index) / 32768.0 - self->axisStates[index];
}

HatDir Joystick_GetHat (Joystick* self, int index) {
  return (HatDir)SDL_JoystickGetHat(self->handle, index);
}

bool Joystick_ButtonDown (Joystick* self, int index) {
  return SDL_JoystickGetButton(self->handle, index) > 0;
}

bool Joystick_ButtonPressed (Joystick* self, int index) {
  return SDL_JoystickGetButton(self->handle, index) > 0 &&
         !self->buttonStates[index];
}

bool Joystick_ButtonReleased (Joystick* self, int index) {
  return SDL_JoystickGetButton(self->handle, index) == 0 &&
         self->buttonStates[index];
}

void Joystick_Update () {
  for (int i = 0; i < kMaxOpen; ++i)
    if (freeList[i])
      Joystick_UpdateSingle(freeList[i]);
}

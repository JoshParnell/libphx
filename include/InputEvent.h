#ifndef PHX_InputEvent
#define PHX_InputEvent

#include "Common.h"
#include "Device.h"

struct InputEvent {
  uint32 timestamp;
  Device device;
  Button button;
  float  value;
  State  state;
};

PHX_API cstr  InputEvent_ToString  (InputEvent*);

#endif

#ifndef PHX_DeviceType
#define PHX_DeviceType

#include "Common.h"

PHX_API const DeviceType DeviceType_Null;
PHX_API const DeviceType DeviceType_Mouse;
PHX_API const DeviceType DeviceType_Keyboard;
PHX_API const DeviceType DeviceType_Gamepad;
const DeviceType DeviceType_COUNT = 4;

PHX_API DeviceType  DeviceType_FromButton  (Button);
PHX_API cstr        DeviceType_ToString    (DeviceType);

#endif

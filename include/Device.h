#ifndef PHX_Device
#define PHX_Device

#include "Common.h"

struct Device {
  DeviceType type;
  uint32     id;
};

PHX_API bool  Device_Equal     (Device*, Device*);
PHX_API cstr  Device_ToString  (Device*);

#endif

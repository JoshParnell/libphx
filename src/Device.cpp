#include "Array.h"
#include "Device.h"
#include "DeviceType.h"

#include <stdio.h>

bool Device_Equal (Device* a, Device* b) {
  return a->type == b->type
      && a->id == b->id;
}

cstr Device_ToString (Device* self) {
  static char buffer[512];

  snprintf(buffer, (size_t) Array_GetSize(buffer), "%s (%u)",
    DeviceType_ToString(self->type), self->id
  );
  return buffer;
}

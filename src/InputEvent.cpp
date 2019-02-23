#include "Array.h"
#include "Button.h"
#include "Device.h"
#include "DeviceType.h"
#include "InputEvent.h"
#include "State.h"

#include <stdio.h>

cstr InputEvent_ToString (InputEvent* ie) {
  static char buffer[512];
  snprintf(buffer, (size_t) Array_GetSize(buffer),
    "Event %p\n"
    "\tTimestamp: %i\n"
    "\tDevice:    %s\n"
    "\tButton:    %s\n"
    "\tValue:     %.2f\n"
    "\tState:     %s",
    ie,
    ie->timestamp,
    Device_ToString(&ie->device),
    Button_ToString(ie->button),
    ie->value,
    State_ToString(ie->state)
  );
  return buffer;
}

#include "Array.h"
#include "Button.h"
#include "DeviceType.h"

#include <stdio.h>

const DeviceType DeviceType_Null     = 0;
const DeviceType DeviceType_Mouse    = 1;
const DeviceType DeviceType_Keyboard = 2;
const DeviceType DeviceType_Gamepad  = 3;

DeviceType DeviceType_FromButton (Button button) {
  return Button_ToDeviceType(button);
}

cstr DeviceType_ToString (DeviceType deviceType) {
  switch (deviceType) {
    default:
      static char buffer[512];
      snprintf(buffer, (size_t) Array_GetSize(buffer), "Unknown (%i)", deviceType);
      return buffer;

    case DeviceType_Null:     return "DeviceType_Null";     break;
    case DeviceType_Mouse:    return "DeviceType_Mouse";    break;
    case DeviceType_Keyboard: return "DeviceType_Keyboard"; break;
    case DeviceType_Gamepad:  return "DeviceType_Gamepad";  break;
  }
}

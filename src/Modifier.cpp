#include "Array.h"
#include "Modifier.h"

#include <stdio.h>

const Modifier Modifier_Null  = 0 << 0;
const Modifier Modifier_Alt   = 1 << 0;
const Modifier Modifier_Ctrl  = 1 << 1;
const Modifier Modifier_Shift = 1 << 2;

cstr Modifier_ToString (Modifier modifier) {
  static char buffer[512];

  if (modifier == Modifier_Null)
    return "Modifier_Null";

  Modifier modifiers[] = {
    Modifier_Alt,
    Modifier_Ctrl,
    Modifier_Shift,
  };
  cstr names[] = {
    "Modifier_Alt",
    "Modifier_Ctrl",
    "Modifier_Shift",
  };

  char* start = buffer;
  const char* sep = "";
  int len = 0;
  for (int i = 0; i < Array_GetSize(modifiers); i++) {
    if (HAS_FLAG(modifier, modifiers[i])) {
      len += snprintf(start + len, Array_GetSize(buffer) - len, "%s%s", sep, names[i]);
      sep = " | ";
      modifier &= ~modifiers[i];
    }
  }

  if (modifier != 0)
    len += snprintf(start + len, Array_GetSize(buffer) - len, "%sUnknown (%i)", sep, modifier);
  return buffer;
}

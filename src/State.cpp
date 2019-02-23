#include "Array.h"
#include "State.h"

#include <stdio.h>

const State State_Null     = 0 << 0;
const State State_Changed  = 1 << 0;
const State State_Pressed  = 1 << 1;
const State State_Down     = 1 << 2;
const State State_Released = 1 << 3;

cstr State_ToString (State state) {
  static char buffer[512];

  if (state == State_Null)
    return "State_Null";

  State states[] = {
    State_Changed,
    State_Pressed,
    State_Down,
    State_Released,
  };
  cstr names[] = {
    "State_Changed",
    "State_Pressed",
    "State_Down",
    "State_Released",
  };

  char* start = buffer;
  const char* sep = "";
  int len = 0;
  for (int i = 0; i < Array_GetSize(states); i++) {
    if (HAS_FLAG(state, states[i])) {
      len += snprintf(start + len, Array_GetSize(buffer) - len, "%s%s", sep, names[i]);
      sep = " | ";
      state &= ~states[i];
    }
  }

  if (state != 0)
    len += snprintf(start + len, Array_GetSize(buffer) - len, "%sUnknown (%i)", sep, state);
  return buffer;
}

/* TODO : Maybe we should have genffi output a ToString function for enums, flags, and structs */

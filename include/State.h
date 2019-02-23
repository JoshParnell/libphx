#ifndef PHX_State
#define PHX_State

#include "Common.h"

PHX_API const State State_Null;
PHX_API const State State_Changed;
PHX_API const State State_Pressed;
PHX_API const State State_Down;
PHX_API const State State_Released;

PHX_API cstr  State_ToString  (State);

#endif

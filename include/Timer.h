#ifndef PHX_Timer
#define PHX_Timer

#include "Common.h"

PHX_API Timer*  Timer_Create();
PHX_API void    Timer_Free         (Timer*);

PHX_API double  Timer_GetAndReset  (Timer*);
PHX_API double  Timer_GetElapsed   (Timer*);
PHX_API void    Timer_Reset        (Timer*);

#endif

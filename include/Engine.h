#ifndef PHX_Engine
#define PHX_Engine

#include "Common.h"

/* --- Engine ------------------------------------------------------------------
 *
 *    Engine_Abort      : Calls abort() (immediate/non-graceful termination)
 *    Engine_GetBits    : Returns 32 for x86-32, 64 for x86-64
 *    Engine_GetTime    : Returns time in seconds since Engine_Init
 *    Engine_GetVersion : Returns pseudo-version string with the date & time of
 *                        the library's compilation
 *    Engine_Terminate  : Calls exit(0)  (graceful termination)
 *
 * -------------------------------------------------------------------------- */

PHX_API void   Engine_Init          (int glVersionMajor, int glVersionMinor);
PHX_API void   Engine_Free          ();

PHX_API void   Engine_Abort         ();
PHX_API int    Engine_GetBits       ();
PHX_API double Engine_GetTime       ();
PHX_API cstr   Engine_GetVersion    ();
PHX_API bool   Engine_IsInitialized ();
PHX_API void   Engine_Terminate     ();
PHX_API void   Engine_Update        ();

#endif

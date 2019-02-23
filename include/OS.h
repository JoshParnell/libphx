#ifndef PHX_OS
#define PHX_OS

#include "Common.h"

PHX_API cstr  OS_GetClipboard    ();
PHX_API int   OS_GetCPUCount     ();
PHX_API cstr  OS_GetVideoDriver  ();
PHX_API void  OS_SetClipboard    (cstr text);

#endif

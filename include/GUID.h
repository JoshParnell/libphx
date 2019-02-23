#ifndef PHX_GUID
#define PHX_GUID

#include "Common.h"

PHX_API uint64  GUID_Create  ();
PHX_API bool    GUID_Exists  (uint64);
PHX_API void    GUID_Reset   ();

#endif

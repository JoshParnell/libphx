#ifndef PHX_Resource
#define PHX_Resource

#include "Common.h"
#include "ResourceType.h"

PHX_API void    Resource_AddPath    (ResourceType, cstr format);

PHX_API bool    Resource_Exists     (ResourceType, cstr name);
PHX_API cstr    Resource_GetPath    (ResourceType, cstr name);
PHX_API Bytes*  Resource_LoadBytes  (ResourceType, cstr name);
PHX_API cstr    Resource_LoadCstr   (ResourceType, cstr name);

/* --- Private API ---------------------------------------------------------- */

PRIVATE void    Resource_Init       ();

#endif

/* TODO : Passing a null name may fail in obscure ways. Determine where we
 *        should check for this to give an appropriate error. (Namely, is it
 *        handled in Resource or File?) */

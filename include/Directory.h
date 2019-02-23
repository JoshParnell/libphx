#ifndef PHX_Directory
#define PHX_Directory

#include "Common.h"

/* --- Directory ---------------------------------------------------------------
 *
 *   An iterator over the contents of the directory.
 *
 *   Directory_Open    : Returns 0 if opening dir fails
 *   Directory_GetNext : Returns 0 once the iterator has traversed all files
 *                     : Automatically skips . and ..
 *
 * -------------------------------------------------------------------------- */

PHX_API Directory*  Directory_Open        (cstr path);
PHX_API void        Directory_Close       (Directory*);
PHX_API cstr        Directory_GetNext     (Directory*);

PHX_API bool        Directory_Change      (cstr cwd);
PHX_API bool        Directory_Create      (cstr path);
PHX_API cstr        Directory_GetCurrent  ();
PHX_API bool        Directory_Remove      (cstr path);

#endif

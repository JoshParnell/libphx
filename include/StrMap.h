#ifndef PHX_StrMap
#define PHX_StrMap

#include "Common.h"

PHX_API StrMap*  StrMap_Create   (uint32 initCapacity);
PHX_API void     StrMap_Free     (StrMap*);
PHX_API void     StrMap_FreeEx   (StrMap*, void (*freeFn)(cstr key, void* value));

PHX_API void*    StrMap_Get      (StrMap*, cstr key);
PHX_API uint32   StrMap_GetSize  (StrMap*);
PHX_API void     StrMap_Remove   (StrMap*, cstr key);
PHX_API void     StrMap_Set      (StrMap*, cstr key, void* val);

PHX_API void     StrMap_Dump     (StrMap*);

PHX_API StrMapIter* StrMap_Iterate      (StrMap*);
PHX_API void        StrMapIter_Advance  (StrMapIter*);
PHX_API void        StrMapIter_Free     (StrMapIter*);
PHX_API bool        StrMapIter_HasMore  (StrMapIter*);
PHX_API cstr        StrMapIter_GetKey   (StrMapIter*);
PHX_API void*       StrMapIter_GetValue (StrMapIter*);

#endif

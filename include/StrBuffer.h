#ifndef PHX_StrBuffer
#define PHX_StrBuffer

#include "Common.h"

PHX_API StrBuffer*  StrBuffer_Create     (uint32 capacity);
PHX_API StrBuffer*  StrBuffer_FromStr    (cstr);
PHX_API void        StrBuffer_Free       (StrBuffer*);

PHX_API void        StrBuffer_Append     (StrBuffer*, StrBuffer*);
PHX_API void        StrBuffer_AppendStr  (StrBuffer*, cstr);
PHX_API void        StrBuffer_Set        (StrBuffer*, cstr format, ...);
PHX_API StrBuffer*  StrBuffer_Clone      (StrBuffer*);
PHX_API cstr        StrBuffer_GetData    (StrBuffer*);

#endif

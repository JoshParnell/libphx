#ifndef PHX_Socket
#define PHX_Socket

#include "Common.h"

PHX_API Socket*  Socket_Create       (SocketType);
PHX_API void     Socket_Free         (Socket*);

PHX_API void     Socket_Bind         (Socket*, int port);
PHX_API cstr     Socket_Read         (Socket*);
PHX_API Bytes*   Socket_ReadBytes    (Socket*);
PHX_API void     Socket_Write        (Socket*, cstr);
PHX_API void     Socket_WriteBytes   (Socket*, Bytes*);

/* TCP-specific. */
PHX_API Socket*  Socket_Accept       (Socket*);
PHX_API void     Socket_Listen       (Socket*);

/* UDP-specific. */
PHX_API int      Socket_ReceiveFrom  (Socket*, void* data, size_t len);
PHX_API cstr     Socket_GetAddress   (Socket*);
PHX_API void     Socket_SetAddress   (Socket*, cstr addr);
PHX_API int      Socket_SendTo       (Socket*, void const* data, size_t len);

#endif

#ifndef PHX_Signal
#define PHX_Signal
#define __FFI_IGNORE__

#include "Common.h"

#ifdef WINDOWS
  #define SIGCALL __CRTDECL
#else
  #define SIGCALL
#endif

typedef int Signal;
typedef void (SIGCALL *SignalHandler)(Signal);

PHX_API const Signal Signal_Int;
PHX_API const Signal Signal_Ill;
PHX_API const Signal Signal_Fpe;
PHX_API const Signal Signal_Segv;
PHX_API const Signal Signal_Term;
PHX_API const Signal Signal_Abrt;

PHX_API void  Signal_Init              ();
PHX_API void  Signal_Free              ();

PHX_API void  Signal_AddHandler        (Signal, SignalHandler);
PHX_API void  Signal_AddHandlerAll     (SignalHandler);
PHX_API void  Signal_RemoveHandler     (Signal, SignalHandler);
PHX_API void  Signal_RemoveHandlerAll  (SignalHandler);
PHX_API cstr  Signal_ToString          (Signal);

/* NOTE : For use only from *within* a handler! */
PHX_API void  Signal_IgnoreDefault     ();

#endif

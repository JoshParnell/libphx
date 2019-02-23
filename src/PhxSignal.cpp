#include "PhxMemory.h"
#include "PhxSignal.h"

#include <signal.h>

/* TODO : Minidump? */

#define SIGNAL_X                                                               \
  X(Signal_Int,  SIGINT,  "Interrupt")                                         \
  X(Signal_Ill,  SIGILL,  "Illegal Instruction")                               \
  X(Signal_Fpe,  SIGFPE,  "FP Exception")                                      \
  X(Signal_Segv, SIGSEGV, "Memory Access Violation")                           \
  X(Signal_Term, SIGTERM, "Terminate")                                         \
  X(Signal_Abrt, SIGABRT, "Abort")

#define X(name, value, desc) const Signal name = value;
SIGNAL_X
#undef X

struct HandlerElem {
  HandlerElem* next;
  SignalHandler fn;
};

static bool ignoreDefault = false;
static SignalHandler handlerDefault[NSIG] = { 0 };
static HandlerElem*  handlerTable[NSIG] = { 0 };

static void SIGCALL Signal_Handler (Signal sig) {
  Warn("Signal_Handler: Caught %s", Signal_ToString(sig));

  /* Re-install default handlers. */ {
#define X(name, value, desc) signal(name, handlerDefault[name]);
  SIGNAL_X
#undef X
  }

  /* Call custom handlers. */ {
    for (HandlerElem* e = handlerTable[sig]; e; e = e->next)
      e->fn(sig);
  }

  if (ignoreDefault) {
    ignoreDefault = false;
    return;
  }

  /* Re-raise the signal to let the default handler run. */
  raise(sig);
}

void Signal_Init () {
#define X(name, value, desc) handlerDefault[name] = signal(name, Signal_Handler);
  SIGNAL_X
#undef X
}

void Signal_Free () {
#define X(name, value, desc) signal(name, handlerDefault[name]);
  SIGNAL_X
#undef X
}

void Signal_AddHandler (Signal sig, SignalHandler fn) {
  HandlerElem* e = MemNew(HandlerElem);
  e->next = handlerTable[sig];
  e->fn = fn;
  handlerTable[sig] = e;
}

void Signal_AddHandlerAll (SignalHandler fn) {
#define X(name, value, desc) Signal_AddHandler(name, fn);
  SIGNAL_X
#undef X
}

void Signal_RemoveHandler (Signal sig, SignalHandler fn) {
  HandlerElem** prev = &handlerTable[sig];
  HandlerElem*  curr =  handlerTable[sig];
  while (curr) {
    if (curr->fn == fn) {
      *prev = curr->next;
      return;
    }
    prev = &curr->next;
    curr =  curr->next;
  }

  Fatal("Signal_RemoveHandler: No such handler installed");
}

void Signal_RemoveHandlerAll (SignalHandler fn) {
#define X(name, value, desc) Signal_RemoveHandler(name, fn);
  SIGNAL_X
#undef X
}

cstr Signal_ToString (Signal self) {
  switch (self) {
#define X(name, value, desc) case name: return desc;
    SIGNAL_X
#undef X
  }
  return "<unknown signal>";
}

void Signal_IgnoreDefault () {
  ignoreDefault = true;
}

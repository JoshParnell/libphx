#include "PhxMemory.h"
#include "SDL.h"
#include "Thread.h"

struct Thread {
  SDL_Thread* handle;
};

Thread* Thread_Create (cstr name, ThreadFn fn, void* data) {
  Thread* self = MemNew(Thread);
  self->handle = SDL_CreateThread(fn, name, data);
  if (!self->handle)
    Fatal("Thread_Create: Failed to start new thread");
  return self;
}

void Thread_Detach (Thread* self) {
  SDL_DetachThread(self->handle);
  MemFree(self);
}

void Thread_Sleep (uint ms) {
  SDL_Delay(ms);
}

int Thread_Wait (Thread* self) {
  int ret;
  SDL_WaitThread(self->handle, &ret);
  MemFree(self);
  return ret;
}

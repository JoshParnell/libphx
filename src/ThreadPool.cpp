#include "PhxMemory.h"
#include "SDL.h"
#include "ThreadPool.h"

struct ThreadData {
  SDL_Thread* handle;
  ThreadPoolFn fn;
  int index;
  int threads;
  void* data;
};

static int ThreadPool_Dispatch (void* data) {
  ThreadData* td = (ThreadData*)data;
  return td->fn(td->index, td->threads, td->data);
};

struct ThreadPool {
  int threads;
  ThreadData* thread;
};

ThreadPool* ThreadPool_Create (int threads) {
  ThreadPool* self = MemNew(ThreadPool);
  self->threads = threads;
  self->thread = MemNewArray(ThreadData, threads);
  for (int i = 0; i < threads; ++i) {
    ThreadData* td = self->thread + i;
    td->handle = 0;
    td->fn = 0;
    td->index = i;
    td->threads = threads;
    td->data = 0;
  }
  return self;
}

void ThreadPool_Free (ThreadPool* self) {
  for (int i = 0; i < self->threads; ++i)
    if (self->thread[i].handle)
      Fatal("ThreadPool_Free: Attempting to free pool with active threads");
  MemFree(self->thread);
  MemFree(self);
}

void ThreadPool_Launch (ThreadPool* self, ThreadPoolFn fn, void* data) {
  for (int i = 0; i < self->threads; ++i) {
    ThreadData* td = self->thread + i;
    td->fn = fn;
    td->data = data;
    td->handle = SDL_CreateThread(ThreadPool_Dispatch, "PHX_ThreadPool", (void*)td);
    if (!td->handle)
      Fatal("ThreadPool_Launch: Failed to start new thread");
  }
}

void ThreadPool_Wait (ThreadPool* self) {
  for (int i = 0; i < self->threads; ++i) {
    ThreadData* td = self->thread + i;
    if (td->handle) {
      int ret;
      SDL_WaitThread(td->handle, &ret);
      td->handle = 0;
    }
  }
}

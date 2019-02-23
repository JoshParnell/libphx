#ifndef PHX_ThreadPool
#define PHX_ThreadPool

#include "Common.h"

typedef int (*ThreadPoolFn)(int threadIndex, int threadCount, void* data);

PHX_API ThreadPool*  ThreadPool_Create  (int threads);
PHX_API void         ThreadPool_Free    (ThreadPool*);

PHX_API void         ThreadPool_Launch  (ThreadPool*, ThreadPoolFn, void* data);
PHX_API void         ThreadPool_Wait    (ThreadPool*);

#endif

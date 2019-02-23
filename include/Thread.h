#ifndef PHX_Thread
#define PHX_Thread

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *  One must choose carefully whether to detach a thread or not. When threads
 *  need to be tightly-synced with the main thread, it is generally more
 *  convenient to keep them attached (as they are by default), and call
 *  Thread_Wait when the main thread needs to ensure that the threaded function
 *  has completed (and to obtain the return code). Threads are not explicitly
 *  freed, since either Thread_Detach or Thread_Wait will end up consuming any
 *  given thread's handle.
 *
 *   Thread_Detach : A detached thread runs its ThreadFn until completion,
 *                   then returns and is automatically destroyed.
 *                 : Consumes the Thread*. A detached thread can no longer be
 *                   accessed by Thread handle. This means it can NOT be waited
 *                   on.
 *                 : Typically used for processes that implement their own loop
 *                   and run independently of the main thread, or asynchronous
 *                   jobs that do not need to be polled for completion.
 *
 *                   Note that it is not possible to obtain the return code
 *                   from a detached thread.
 *
 *   Thread_Wait   : For non-detached threads, wait on the thread to complete
 *                   and then return the corresponding ThreadFn return code.
 *                 : Consumes the Thread*.
 *
 * -------------------------------------------------------------------------- */

typedef int (*ThreadFn)(void* data);

PHX_API Thread*  Thread_Create  (cstr name, ThreadFn, void* data);
PHX_API void     Thread_Detach  (Thread*);
PHX_API void     Thread_Sleep   (uint ms);
PHX_API int      Thread_Wait    (Thread*);

#endif

#ifndef PHX_PhxConfig
#define PHX_PhxConfig

/* GLCHECK inserts a GL driver error query after every OpenGL call that can
 * set an error. Extremely slow but extremely effective at verifying correctness
 * of GL API usage. */
#define ENABLE_GLCHECK 0

/* The internal profiler has near-zero overhead, but turning it off can still
 * be useful to periodically ensure that frames are not causing overhead. */
#define ENABLE_PROFILER 1

/* Causes the internal profiler to produce a detailed event trace that can be
 * inspected by chrome://tracing. Producing trace output adds significant
 * overhead to each profiled frame, so trace output should be turned on only
 * when the output is actually needed. */
#define ENABLE_PROFILER_TRACE 0

/* TODO AB : Brief summary of this flag (hoisted from BSP.cpp) */
#define ENABLE_BSP_PROFILING 0

#endif

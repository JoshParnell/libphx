#include "ArrayList.h"
#include "HashMap.h"
#include "PhxMemory.h"
#include "PhxMath.h"
#include "PhxSignal.h"
#include "Profiler.h"
#include "PhxString.h"
#include "TimeStamp.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_DEPTH 128
#define RESERVE_SIZE 1024
#define TRACE_FILE "log/trace.json"

struct Scope {
  cstr name;
  TimeStamp last;
  TimeStamp frame;
  TimeStamp total;
  double count;
  double mean;
  double var;
  double min;
  double max;
};

struct Profiler {
  HashMap* map;
  int stackIndex;
  Scope* stack[MAX_STACK_DEPTH];
  ArrayList(Scope*, scopeList);
  TimeStamp start;
#if ENABLE_PROFILER_TRACE
  FILE* traceLog;
#endif
} static self;

static bool profiling = false;

#if ENABLE_PROFILER_TRACE
static void Trace_Start () {
  self.traceLog = fopen(TRACE_FILE, "wb");
  fputs("[ {"
    "\"name\":\"TRACE_START\","
    "\"ph\":\"M\","
    "\"pid\":1,\"tid\":1}", self.traceLog);
}

static void Trace_Stop () {
  fputs("\n]", self.traceLog);
  fclose(self.traceLog);
  self.traceLog = 0;
}

inline static void Trace_Begin (cstr name, TimeStamp ts) {
  fprintf(self.traceLog, ",\n  {"
    "\"name\":\"%s\","
    "\"ph\":\"B\","
    "\"ts\":%llu,"
    "\"pid\":1,\"tid\":1"
    "}",
    name, ts);
}

inline static void Trace_End (cstr name, TimeStamp ts) {
  fprintf(self.traceLog, ",\n  {"
    "\"name\":\"%s\","
    "\"ph\":\"E\","
    "\"ts\":%llu,"
    "\"pid\":1,\"tid\":1"
    "}",
    name, ts);
}
#endif

static Scope* Scope_Create (cstr name) {
  Scope* scope = MemNew(Scope);
  scope->name = StrDup(name);
  scope->last = 0;
  scope->frame = 0;
  scope->total = 0;

  scope->count = 0.0;
  scope->mean = 0.0;
  scope->var = 0.0;
  scope->min = 1e30;
  scope->max = -1e30;
  ArrayList_Append(self.scopeList, scope);
  return scope;
}

static void Scope_Free (Scope* scope) {
  StrFree(scope->name);
  MemFree(scope);
}

static int SortScopes (void const* pa, void const* pb) {
  Scope const* a = *(Scope const**)pa;
  Scope const* b = *(Scope const**)pb;
  return b->total <  a->total ? -1 :
         b->total == a->total ?  0 : 1;
}

static Scope* Profiler_GetScope (cstr name) {
  Scope* scope = (Scope*)HashMap_GetRaw(self.map, (uint64)(size_t)name);
  if (scope) return scope;
  scope = Scope_Create(name);
  HashMap_SetRaw(self.map, (uint64)(size_t)name, scope);
  return scope;
}

static void Profiler_SignalHandler (Signal) {
  Profiler_Backtrace();
}

/* -------------------------------------------------------------------------- */

void Profiler_Enable () {
#if ENABLE_PROFILER
  profiling = true;
  self.map = HashMap_Create(sizeof(void*), 2 * RESERVE_SIZE);
  ArrayList_Init(self.scopeList);
  ArrayList_Reserve(self.scopeList, RESERVE_SIZE);
  self.stackIndex = -1;
  self.start = TimeStamp_Get();
#if ENABLE_PROFILER_TRACE
  Trace_Start();
#endif
  Profiler_Begin("[Root]");
  Signal_AddHandlerAll(Profiler_SignalHandler);
#endif
}

void Profiler_Disable () {
#if ENABLE_PROFILER
  if (self.stackIndex != 0)
    Fatal("Profiler_Disable: Cannot stop profiler from within a profiled section");

  Profiler_End();
#if ENABLE_PROFILER_TRACE
  Trace_Stop();
#endif
  double total = TimeStamp_GetElapsed(self.start);

  /* Compute compensated variance. */
  for (int i = 0; i < ArrayList_GetSize(self.scopeList); ++i) {
    Scope* scope = ArrayList_Get(self.scopeList, i);
    scope->var /= scope->count - 1.0;
    scope->var = Sqrt(scope->var);
  }

  qsort(
    ArrayList_GetData(self.scopeList),
    ArrayList_GetSize(self.scopeList),
    sizeof(Scope*),
    SortScopes);

  puts("-- PHX PROFILER -------------------------------------");
  double cumulative = 0;
  for (int i = 0; i < ArrayList_GetSize(self.scopeList); ++i) {
    Scope* scope = ArrayList_Get(self.scopeList, i);
    double scopeTotal = TimeStamp_ToDouble(scope->total);
    cumulative += scopeTotal;
    if ((scopeTotal / total) < 0.01 && scope->max < 0.01)
      continue;
    printf("%*.1f%% %*.0f%% %*.0fms  [%*.2f, %*.2f] %*.2f  / %*.2f  (%*.0f%%)  |  %s\n",
      5, 100.0 * (scopeTotal / total),
      4, 100.0 * (cumulative / total),
      6, 1000.0 * scopeTotal,
      6, 1000.0 * scope->min,
      6, 1000.0 * scope->max,
      6, 1000.0 * scope->mean,
      5, 1000.0 * scope->var,
      4, 100.0 * (scope->var / scope->mean),
      scope->name);
  }
  puts("-----------------------------------------------------");
  fflush(stdout);

  ArrayList_FreeEx(self.scopeList, Scope_Free);
  HashMap_Free(self.map);
  profiling = false;
  Signal_RemoveHandlerAll(Profiler_SignalHandler);
#endif
}

void Profiler_Begin (cstr name) {
#if ENABLE_PROFILER
  if (!profiling) return;
  if (self.stackIndex + 1 >= MAX_STACK_DEPTH) {
    Profiler_Backtrace();
    Fatal("Profiler_Begin: Maximum stack depth exceeded");
  }
  TimeStamp now = TimeStamp_Get();

  if (self.stackIndex >= 0) {
    Scope* prev = self.stack[self.stackIndex];
    prev->frame += now - prev->last;
    prev->last = now;
  }

  self.stackIndex++;
  Scope* curr = Profiler_GetScope(name);
  self.stack[self.stackIndex] = curr;
  curr->last = now;

#if ENABLE_PROFILER_TRACE
  Trace_Begin(curr->name, now);
#endif
#endif
}

void Profiler_End () {
#if ENABLE_PROFILER
  if (!profiling) return;
  if (self.stackIndex < 0) {
    Profiler_Backtrace();
    Fatal("Profiler_End: Attempting to pop an empty stack");
  }
  TimeStamp now = TimeStamp_Get();
  Scope* prev = self.stack[self.stackIndex];
  prev->frame += now - prev->last;
  self.stackIndex--;

#if ENABLE_PROFILER_TRACE
  Trace_End(prev->name, now);
#endif

  if (self.stackIndex >= 0) {
    Scope* curr = self.stack[self.stackIndex];
    curr->last = now;
  }
#endif
}

void Profiler_SetValue (cstr name, int value) {
#if ENABLE_PROFILER
#if ENABLE_PROFILER_TRACE
  if (!profiling) return;
  TimeStamp ts = TimeStamp_Get();
  fprintf(self.traceLog, ",\n  {"
    "\"name\":\"%s\","
    "\"ph\":\"C\","
    "\"ts\":%llu,"
    "\"args\":{\"value\": %d},"
    "\"pid\":1,\"tid\":1"
    "}",
    name, ts, value);
#endif
#endif
}

void Profiler_LoopMarker () {
#if ENABLE_PROFILER
  if (!profiling) return;
  for (int i = 0; i < ArrayList_GetSize(self.scopeList); ++i) {
    Scope* scope = ArrayList_Get(self.scopeList, i);
    if (scope->frame > 0.0) {
      scope->total += scope->frame;
      double frame = TimeStamp_ToDouble(scope->frame);
      scope->min = Min(scope->min, frame);
      scope->max = Max(scope->max, frame);

      /* Use Welford's algorithm to compute variance in one pass. */
      scope->count += 1.0;
      double d1 = frame - scope->mean;
      scope->mean += d1 / scope->count;
      double d2 = frame - scope->mean;
      scope->var += d1 * d2;
      scope->frame = 0;
    }
  }
#endif
}

void Profiler_Backtrace () {
#if ENABLE_PROFILER
  if (!profiling) return;
  puts("PHX Profiler Backtrace:");
  for (int i = 0; i <= self.stackIndex; ++i) {
    int index = self.stackIndex - i;
    printf("  [%i] %s\n", index, self.stack[index]->name);
  }
  fflush(stdout);
#endif
}

/* -------------------------------------------------------------------------- */

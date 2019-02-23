#include "ArrayList.h"
#include "Lua.h"
#include "TimeStamp.h"

#include <stdlib.h>

/* WARNING : Scheduler is currently global, hence the entire scheduling
 *           mechanism may be used from only a single Lua instance! We can
 *           improve this in the future. */

/* NOTE : Should qsort ever become the bottleneck here, we can exploit the
 *        temporal coherence of this queue in numerous ways. Heaps, segregated
 *        unsorted time range interval blocks, modular/radix buckets, etc. */

typedef lua_Integer LuaRef;

struct SchedulerElem {
  LuaRef fn;
  LuaRef arg;
  TimeStamp tCreated;
  TimeStamp tWake;
};

struct Scheduler {
  ArrayList(SchedulerElem, elems);
  ArrayList(SchedulerElem, addQueue);
  TimeStamp now;
  bool locked;
} static self;

/* NOTE : We sort in decreasing order of wake time, such that the element with
 *        the earliest wake time is at the end. This way, we can pop elements
 *        off the stack as we process them in O(1) without breaking order. */
static int SortByWake (void const* pa, void const* pb) {
  SchedulerElem const* a = (SchedulerElem const*)pa;
  SchedulerElem const* b = (SchedulerElem const*)pb;
  return a->tWake <  b->tWake ? 1 :
         a->tWake == b->tWake ? 0 : -1;
}

/* (fn, arg, timeUntilWake) */
static int LuaScheduler_Add (Lua* L) {
  SchedulerElem elem;

  /* Compute timestamps. */ {
    double timeToWake = lua_tonumber(L, lua_gettop(L));
    elem.tCreated = self.now;
    elem.tWake = TimeStamp_GetRelative(self.now, timeToWake);
    lua_pop(L, 1);
  }

  /* Get references to arg and fn. */ {
    elem.arg = Lua_GetRef(L);
    elem.fn = Lua_GetRef(L);
  }

  /* Queue the element. */ {
    if (self.locked) {
      ArrayList_Append(self.addQueue, elem);
    } else {
      ArrayList_Append(self.elems, elem);
    }
  }

  return 0;
}

static int LuaScheduler_Clear (Lua* L) {
  /* Release all references. */ {
    for (int i = 0; i < ArrayList_GetSize(self.elems); ++i) {
      SchedulerElem* elem = ArrayList_GetPtr(self.elems, i);
      luaL_unref(L, LUA_REGISTRYINDEX, elem->fn);
      luaL_unref(L, LUA_REGISTRYINDEX, elem->arg);
    }
  }

  ArrayList_Clear(self.elems);
  return 0;
}

static int LuaScheduler_Update (Lua* L) {
  /* Lock the element queue while we process to preserve ordering. */
  self.locked = true;

  qsort(
    ArrayList_GetData(self.elems),
    ArrayList_GetSize(self.elems),
    sizeof(SchedulerElem),
    SortByWake);

  self.now = TimeStamp_Get();

  /* Push the error handler. */
  lua_getglobal(L, "__error_handler__");
  int handler = lua_gettop(L);

  while (ArrayList_GetSize(self.elems)) {
    SchedulerElem* elem = ArrayList_GetLastPtr(self.elems);
    if (self.now < elem->tWake)
      break;

    double dt = TimeStamp_GetDifference(elem->tCreated, self.now);

    /* Call fn(dt, arg) */ {
      Lua_PushRef(L, elem->fn);
      Lua_PushNumber(L, dt);
      Lua_PushRef(L, elem->arg);
      Lua_Call(L, 2, 0, handler);
    }

    /* Release references on fn and arg. */
    Lua_ReleaseRef(L, elem->fn);
    Lua_ReleaseRef(L, elem->arg);
    ArrayList_Pop(self.elems);
  }

  /* Pop the error handler. */
  lua_pop(L, 1);

  self.locked = false;

  /* Append any elements that were added while locked. */
  while (ArrayList_GetSize(self.addQueue)) {
    SchedulerElem elem = ArrayList_PopRet(self.addQueue);
    ArrayList_Append(self.elems, elem);
  }

  return 0;
}

void LuaScheduler_Init (Lua* L) {
  UNUSED(L);
  /* TODO : Store in Lua state so that the scheduler is per-instance */
  ArrayList_Init(self.elems);
  ArrayList_Init(self.addQueue);
  self.now = TimeStamp_Get();
  self.locked = false;
}

void LuaScheduler_Register (Lua* L) {
  Lua_SetFn(L, "Schedule", LuaScheduler_Add);
  Lua_SetFn(L, "SchedulerClear", LuaScheduler_Clear);
  Lua_SetFn(L, "SchedulerUpdate", LuaScheduler_Update);
}

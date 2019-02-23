#include "ArrayList.h"
#include "Lua.h"
#include "PhxMemory.h"
#include "PhxSignal.h"
#include "PhxString.h"
#include "Profiler.h"
#include "Resource.h"

cstr const kErrorHandler =
  "function __error_handler__ (e)"
  "  return debug.traceback(e, 1)"
  "end"
;

static bool initialized = false;
static Lua* activeInstance = 0;
static Signal cSignal = 0;

static void Lua_BacktraceHook (Lua *self, lua_Debug*) {
  lua_sethook(self, 0, 0, 0);
  luaL_where(self, 0);
  lua_pushstring(self, StrAdd("Received Signal: ", Signal_ToString(cSignal)));
  lua_error(self);
}

static void Lua_SignalHandler (Signal sig) {
  if (!activeInstance) return;
  if (sig == Signal_Abrt || sig == Signal_Segv) {
    /* NOTE : The implementation of abort() causes the program to forcefully
     *        exit as soon as the signal handler returns. Thus Lua_BacktraceHook
     *        will never get a chance to be called and we have to dump a trace
     *        now. */
    Lua_Backtrace();
  } else {
    cSignal = sig;
    lua_sethook(activeInstance, Lua_BacktraceHook, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
    Signal_IgnoreDefault();
  }
}

static void Lua_PCall (Lua* self, int args, int rets, int errorHandler) {
  Lua* prev = activeInstance;
  activeInstance = self;
  int result = lua_pcall(self, args, rets, errorHandler);
  if (result != 0) {
    if (result == LUA_ERRMEM)
      Fatal("Lua_PCall: Lua returned a memory allocation error");
    else if (result == LUA_ERRERR)
      Fatal("Lua_PCall: Lua errored while attempting to run the error handler");
    else if (result == LUA_ERRRUN) {
      cstr error = lua_tostring(self, -1);
      Fatal("Lua_PCall: Lua returned error message: %s", error);
    } else {
      Fatal("Lua_PCall: Lua returned an invalid error code (corruption?)");
    }
  }
  activeInstance = prev;
}

/* -- Utility CFunctions to be injected into global state ------------------- */

static int Lua_CallBarrier (Lua* self) {
  int args = lua_gettop(self) - 1;
  lua_call(self, args, LUA_MULTRET);
  return lua_gettop(self);
}

/* -------------------------------------------------------------------------- */

static void Lua_InitExtensions (Lua* self) {
  Lua_SetFn(self, "Call", Lua_CallBarrier);
  LuaScheduler_Init(self);
  LuaScheduler_Register(self);
}

Lua* Lua_Create () {
  if (!initialized) {
    initialized = true;
    Signal_AddHandlerAll(Lua_SignalHandler);
  }

  Lua* self = luaL_newstate();
  luaL_openlibs(self);
  Lua_InitExtensions(self);
  if (luaL_dostring(self, kErrorHandler))
    Fatal("Lua_Create: failed to load error handler");
  return self;
}

Lua* Lua_CreateThread (Lua* self) {
  return lua_newthread(self);
}

void Lua_Free (Lua* self) {
  lua_close(self);
}

Lua* Lua_GetActive () {
  return activeInstance;
}

void Lua_DoFile (Lua* self, cstr name) {
  Lua_LoadFile(self, name);
  Lua_PCall(self, 0, 0, 0);
}

void Lua_DoString (Lua* self, cstr code) {
  Lua_LoadString(self, code);
  Lua_PCall(self, 0, 0, 0);
}

void Lua_LoadFile (Lua* self, cstr name) {
  cstr path = Resource_GetPath(ResourceType_Script, name);
  if (luaL_loadfile(self, path))
    Fatal("Lua_LoadFile: failed to load <%s>:\n%s", path, lua_tostring(self, -1));
}

void Lua_LoadString (Lua* self, cstr code) {
  if (luaL_loadstring(self, code))
    Fatal("Lua_LoadString: failed to load string:\n%s", lua_tostring(self, -1));
}

void Lua_Call (Lua* self, int args, int rets, int errorHandler) {
  Lua_PCall(self, args, rets, errorHandler);
}

void Lua_PushGlobal (Lua* self, cstr name) {
  lua_getfield(self, LUA_GLOBALSINDEX, name);
  if (lua_isnil(self, lua_gettop(self)))
    Fatal("Lua_PushGlobal: failed to find global key <%s>", name);
}

void Lua_PushNumber (Lua* self, double value) {
  lua_pushnumber(self, value);
}

void Lua_PushPtr (Lua* self, void* value) {
  lua_pushlightuserdata(self, value);
}

void Lua_PushStr (Lua* self, cstr value) {
  lua_pushstring(self, value);
}

void Lua_PushThread (Lua* self, Lua* thread) {
  lua_pushthread(thread);
  lua_xmove(thread, self, 1);
}

void Lua_SetBool (Lua* self, cstr name, bool value) {
  lua_pushboolean(self, value);
  lua_setglobal(self, name);
}

void Lua_SetFn (Lua* self, cstr name, LuaFn fn) {
  lua_register(self, name, fn);
}

void Lua_SetNumber (Lua* self, cstr name, double value) {
  lua_pushnumber(self, value);
  lua_setglobal(self, name);
}

void Lua_SetPtr (Lua* self, cstr name, void* value) {
  lua_pushlightuserdata(self, value);
  lua_setglobal(self, name);
}

void Lua_SetStr (Lua* self, cstr name, cstr value) {
  lua_pushstring(self, value);
  lua_setglobal(self, name);
}

void Lua_TransferStack (Lua* src, Lua* dst, int count) {
  lua_xmove(src, dst, count);
}

LuaRef Lua_GetRef (Lua* self) {
  return luaL_ref(self, LUA_REGISTRYINDEX);
}

void Lua_ReleaseRef (Lua* self, LuaRef ref) {
  luaL_unref(self, LUA_REGISTRYINDEX, ref);
}

void Lua_PushRef (Lua* self, LuaRef ref) {
  lua_rawgeti(self, LUA_REGISTRYINDEX, ref);
}

void Lua_GCFull (Lua* self) {
  lua_gc(self, LUA_GCCOLLECT, 0);
}

void Lua_GCSetActive (Lua* self, bool active) {
  if (active)
    lua_gc(self, LUA_GCRESTART, 0);
  else
    lua_gc(self, LUA_GCSTOP, 0);
}

void Lua_GCStep (Lua* self) {
  lua_gc(self, LUA_GCSTEP, 0);
}

int Lua_GetMemory (Lua* self) {
  return lua_gc(self, LUA_GCCOUNT, 0) * 1024 +
         lua_gc(self, LUA_GCCOUNTB, 0);
}

inline static cstr Lua_ToString (Lua* self, cstr name) {
  /* NOTE : We leak strings allocated by StrDup/Format in a couple places here
   *        to simplify the code. We're in the middle of crashing so it doesn't
   *        matter. */

  /* NOTE : LUA_TCDATA and a couple other extensions are defined in lj_obj.h.
   *        Pulling that in requires pulling in a bunch of stuff. */
  #define LUA_TCDATA 10

  int  type     = lua_type(self, -1);
  cstr typeName = lua_typename(self, type);
  cstr strValue = 0;
  bool isNull   = false;

  if (luaL_callmeta(self, -1, "__tostring")) {
    strValue = StrDup(lua_tostring(self, -1));
    lua_pop(self, 1);
  } else {
    switch (type) {
      default: Fatal("Lua_ToString: Unexpected type %i", type);

      case LUA_TNIL:           strValue = "nil"; isNull = true;                        break;
      case LUA_TBOOLEAN:       strValue = lua_toboolean (self, -1) ? "True" : "False"; break;
      case LUA_TNUMBER:        strValue = lua_tostring  (self, -1);                    break;
      case LUA_TSTRING:        strValue = lua_tostring  (self, -1);                    break;
      case LUA_TLIGHTUSERDATA: strValue = StrFormat("0x%p", lua_touserdata(self, -1)); break;
      case LUA_TTABLE:         strValue = StrFormat("0x%p", lua_topointer (self, -1)); break;
      case LUA_TFUNCTION:      strValue = StrFormat("0x%p", lua_topointer (self, -1)); break;
      case LUA_TUSERDATA:      strValue = StrFormat("0x%p", lua_touserdata(self, -1)); break;
      case LUA_TTHREAD:        strValue = StrFormat("0x%p", lua_topointer (self, -1)); break;
      case LUA_TCDATA:         strValue = StrFormat("0x%p", lua_topointer (self, -1)); break;
    }
  }

  cstr pre = isNull ? "[91;1m" : "";
  cstr app = isNull ? "[0m"    : "";
  return StrFormat("%s      %-10s %-16s = %s%s", pre, typeName, name, strValue, app);
}

void Lua_Backtrace () {
  Lua* self = activeInstance;
  if (!self) return;


  /* NOTE : This is inefficient, but meh it's happening during a crash... */
  ArrayList(cstr, stack); ArrayList_Init(stack);
  for (int iStack = 0; true; iStack++) {
    lua_Debug ar = {};
    int result = lua_getstack(self, iStack, &ar);
    if (result == 0) break;

    result = lua_getinfo(self, "nSluf", &ar);
    if (result == 0)
      Fatal("Lua_GetStack: lua_getinfo failed.");

    int variablesPrinted = 0;


    { //File, line, and function
      cstr  funcName = ar.name;
      cstr  fileName = ar.source;
      int32 line     = ar.currentline;
      if (fileName[0] != '@')      { fileName = "<string>"; line = -1; }
      if (fileName[0] == '@')        fileName = fileName + 1;
      if (StrEqual(ar.what, "C"))    fileName = "<native>";
      if (StrEqual(ar.what, "main")) funcName = "<main>";
      if (!funcName)                 funcName = "<null>";

      cstr stackFrame = (line > 0)
        ? StrFormat("  %s(%i): %s", fileName, line, funcName)
        : StrFormat("  %s: %s", fileName, funcName);
      ArrayList_Append(stack, stackFrame);
    }


    { //Upvalues
      for (int iUp = 1; true; iUp++) {
        cstr name = lua_getupvalue(self, -1, iUp);
        if (!name) break;

        if (iUp == 1)
          ArrayList_Append(stack, StrDup("    [Upvalues]"));

        cstr upValue = Lua_ToString(self, name);
        ArrayList_Append(stack, upValue);
        lua_pop(self, 1);
        variablesPrinted++;
      }
    }


    { //Locals
      for (int iLocal = 1; true; iLocal++) {
        cstr name = lua_getlocal(self, &ar, iLocal);
        if (!name) break;

        if (iLocal == 1)
          ArrayList_Append(stack, StrDup("    [Locals]"));

        cstr local = Lua_ToString(self, name);
        ArrayList_Append(stack, local);
        lua_pop(self, 1);
        variablesPrinted++;
      }
    }

    if (variablesPrinted > 0)
      ArrayList_Append(stack, StrDup(""));
    lua_pop(self, 1);
  }

  Warn("Lua Backtrace:");
  ArrayList_ForEach(stack, cstr, stackFrame) {
    Warn(*stackFrame);
    StrFree(*stackFrame);
  }
  ArrayList_Free(stack);
}

/* TODO : Add backtrace to lua errors in PCall */

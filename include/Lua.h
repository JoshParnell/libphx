#ifndef PHX_Lua
#define PHX_Lua
#define __FFI_IGNORE__

#include "Common.h"
#include "luajit/lua.hpp"

typedef lua_State Lua;
typedef int (*LuaFn)(Lua*);
typedef lua_Integer LuaRef;

PHX_API Lua*    Lua_Create             ();
PHX_API Lua*    Lua_CreateThread       (Lua*);
PHX_API void    Lua_Free               (Lua*);
PHX_API Lua*    Lua_GetActive          ();

PHX_API void    Lua_DoFile             (Lua*, cstr name);
PHX_API void    Lua_DoString           (Lua*, cstr code);

PHX_API void    Lua_LoadFile           (Lua*, cstr name);
PHX_API void    Lua_LoadString         (Lua*, cstr code);

PHX_API void    Lua_Call               (Lua*, int args, int rets, int errorHandler);
PHX_API void    Lua_PushGlobal         (Lua*, cstr name);
PHX_API void    Lua_PushNumber         (Lua*, double);
PHX_API void    Lua_PushPtr            (Lua*, void*);
PHX_API void    Lua_PushStr            (Lua*, cstr);
PHX_API void    Lua_PushThread         (Lua*, Lua* thread);
PHX_API void    Lua_SetBool            (Lua*, cstr name, bool);
PHX_API void    Lua_SetFn              (Lua*, cstr name, LuaFn);
PHX_API void    Lua_SetNumber          (Lua*, cstr name, double);
PHX_API void    Lua_SetPtr             (Lua*, cstr name, void*);
PHX_API void    Lua_SetStr             (Lua*, cstr name, cstr);
PHX_API void    Lua_TransferStack      (Lua* src, Lua* dst, int count);

PHX_API LuaRef  Lua_GetRef             (Lua*);
PHX_API void    Lua_ReleaseRef         (Lua*, LuaRef);
PHX_API void    Lua_PushRef            (Lua*, LuaRef);

PHX_API void    Lua_GCFull             (Lua*);
PHX_API void    Lua_GCSetActive        (Lua*, bool active);
PHX_API void    Lua_GCStep             (Lua*);

PHX_API void    Lua_Backtrace          ();
PHX_API int     Lua_GetMemory          (Lua*);

/* --- Private API ---------------------------------------------------------- */

PRIVATE void    LuaScheduler_Init      (Lua*);
PRIVATE void    LuaScheduler_Register  (Lua*);


#endif

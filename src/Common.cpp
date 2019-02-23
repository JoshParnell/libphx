#include "Common.h"
#include "Lua.h"
#include "PhxMemory.h"
#include "PhxString.h"
#include "Profiler.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WINDOWS
  #include <windows.h>

  static void Fatal_Output (cstr message) {
    /* TODO : We want this in Visual Studio where stdout/err don't go to the
     *       Console window, but not in VS Code where it does. */
    //OutputDebugStringA(message);
    fprintf(stderr, "%s\n", message);

    #ifndef BUILD_DISTRIBUTABLE
      int result = MessageBox(0,
        StrAdd(message, "\n\nBreak into debugger? (Cancel to continue execution)"),
        "PHX Engine: Fatal Error", MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON2);
      switch(result) {
        case IDYES:
          Profiler_Backtrace();
          Lua_Backtrace();
          DEBUG_BREAK;
          exit(1);
          break;

        case IDNO:
          abort();
          break;

        case IDCANCEL:
          break;
      }
    #else
      abort();
    #endif
  }
#else
  static void Fatal_Output (cstr message) {
    puts(message);
    abort();
  }
#endif

void Fatal (cstr format, ...) {
  va_list args;
  va_start(args, format);
  int len = vsnprintf(0, 0, format, args) + 1;
  va_end(args);

  char* message = MemNewArray(char, len);
  va_start(args, format);
  vsnprintf(message, len, format, args);
  va_end(args);

  Fatal_Output(message);
}

void Warn (cstr format, ...) {
  va_list args;
  va_start(args, format);
  int len = vsnprintf(0, 0, format, args) + 1;
  va_end(args);

  char* message = MemNewArray(char, len);
  va_start(args, format);
  vsnprintf(message, len, format, args);
  va_end(args);

  fprintf(stdout, "%s\n", message);
  fflush(stdout);
  free(message);
}

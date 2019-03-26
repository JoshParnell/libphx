#include "Directory.h"
#include "Engine.h"
#include "Gamepad.h"
#include "Input.h"
#include "Joystick.h"
#include "Keyboard.h"
#include "Metric.h"
#include "Mouse.h"
#include "PhxSignal.h"
#include "Profiler.h"
#include "Resource.h"
#include "SDL.h"
#include "ShaderVar.h"
#include "TimeStamp.h"

#include <stdio.h>

/* On Windows, request usage of the dedicated GPU if the machine switches
 * between on-board and dedicated GPUs dynamically. Only works when exported
 * by the exe, not when exported by a dll. */
#if WINDOWS
  extern "C" {
    __declspec(dllexport) ulong NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
  }
#endif

const uint32 subsystems =
  SDL_INIT_EVENTS |
  SDL_INIT_VIDEO |
  SDL_INIT_TIMER |
  SDL_INIT_HAPTIC |
  SDL_INIT_JOYSTICK |
  SDL_INIT_GAMECONTROLLER;

static TimeStamp initTime = 0;

void Engine_Init (int glVersionMajor, int glVersionMinor) {
  static bool firstTime = true;
  Signal_Init();

  if (firstTime) {
    firstTime = false;
    /* Check SDL version compatibility. */ {
      SDL_version compiled;
      SDL_version linked;
      SDL_VERSION(&compiled);
      SDL_GetVersion(&linked);
      if (compiled.major != linked.major ||
          compiled.minor != linked.minor ||
          compiled.patch != linked.patch)
      {
        puts("Engine_Init: Detected SDL version mismatch:");
        printf("  Version (Compiled) : %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
        printf("  Version (Linked)   : %d.%d.%d\n", linked.major, linked.minor, linked.patch);
        Fatal("Engine_Init: Terminating.");
      }
    }

    if (SDL_Init(0) != 0)
      Fatal("Engine_Init: Failed to initialize SDL");
    if (!Directory_Create("log"))
      Fatal("Engine_Init: Failed to create log directory.");
    atexit(SDL_Quit);
  }

  if (SDL_InitSubSystem(subsystems) != 0)
    Fatal("Engine_Init: Failed to initialize SDL's subsystems");

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glVersionMajor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glVersionMinor);
  SDL_GL_SetAttribute(
    SDL_GL_CONTEXT_PROFILE_MASK,
    SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  Keyboard_Init();
  Metric_Reset();
  Mouse_Init();
  Input_Init();
  Resource_Init();
  ShaderVar_Init();

  initTime = TimeStamp_Get();
}

void Engine_Free () {
  ShaderVar_Free();
  Keyboard_Free();
  Mouse_Free();
  Input_Free();
  Signal_Free();
  SDL_QuitSubSystem(subsystems);
}

void Engine_Abort () {
  abort();
}

int Engine_GetBits () {
  return (int)(8 * sizeof(void*));
}

double Engine_GetTime () {
  return TimeStamp_GetElapsed(initTime);
}

bool Engine_IsInitialized () {
  return initTime != 0;
}

void Engine_Terminate () {
  exit(0);
}

void Engine_Update () {
  FRAME_BEGIN;
  Metric_Reset();
  Keyboard_UpdatePre();
  Mouse_Update();
  Joystick_Update();
  Gamepad_Update();
  Input_Update();
  Keyboard_UpdatePost();
  FRAME_END;
}

#include "PhxMemory.h"
#include "SDL.h"
#include "Timer.h"

struct Timer {
  uint64 value;
};

static double frequency = 0;

Timer* Timer_Create () {
  static bool init = false;
  if (!init) {
    init = true;
    frequency = (double)SDL_GetPerformanceFrequency();
  }

  Timer* self = MemNew(Timer);
  self->value = SDL_GetPerformanceCounter();
  return self;
}

void Timer_Free (Timer* self) {
  MemFree(self);
}

double Timer_GetAndReset (Timer* self) {
  uint64 now = SDL_GetPerformanceCounter();
  double elapsed = (double)(now - self->value) / frequency;
  self->value = now;
  return elapsed;
}

double Timer_GetElapsed (Timer* self) {
  uint64 now = SDL_GetPerformanceCounter();
  return (double)(now - self->value) / frequency;
}

void Timer_Reset (Timer* self) {
  self->value = SDL_GetPerformanceCounter();
}

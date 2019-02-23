#include "SDL.h"
#include "TimeStamp.h"

TimeStamp TimeStamp_Get () {
  return SDL_GetPerformanceCounter();
}

double TimeStamp_GetDifference (TimeStamp start, TimeStamp end) {
  double freq = (double)SDL_GetPerformanceFrequency();
  return (double)(end - start) / freq;
}

double TimeStamp_GetElapsed (TimeStamp then) {
  double freq = (double)SDL_GetPerformanceFrequency();
  return (double)(SDL_GetPerformanceCounter() - then) / freq;
}

double TimeStamp_GetElapsedMs (TimeStamp then) {
  double freq = (double)SDL_GetPerformanceFrequency();
  return (double)(1000 * (SDL_GetPerformanceCounter() - then)) / freq;
}

TimeStamp TimeStamp_GetFuture (double seconds) {
  double freq = (double)SDL_GetPerformanceFrequency();
  return SDL_GetPerformanceCounter() + (TimeStamp)(freq * seconds);
}

TimeStamp TimeStamp_GetRelative (TimeStamp start, double seconds) {
  double freq = (double)SDL_GetPerformanceFrequency();
  return start + (TimeStamp)(freq * seconds);
}

/* TODO : Keep the division in uint64, only use double for remainder. */
double TimeStamp_ToDouble (TimeStamp self) {
  return (double)self / (double)SDL_GetPerformanceFrequency();
}

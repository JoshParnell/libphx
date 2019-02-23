#include "Keyboard.h"
#include "PhxMemory.h"
#include "SDL.h"

static uint64 lastAction;
static uchar* stateLast;
static uchar* stateCurr;

void Keyboard_Init () {
  int size;
  uchar const* state = SDL_GetKeyboardState(&size);
  stateLast = MemNewArray(uchar, size);
  stateCurr = MemNewArray(uchar, size);
  MemCpy(stateLast, state, size);
  MemCpy(stateCurr, state, size);
  lastAction = SDL_GetPerformanceCounter();
}

void Keyboard_Free () {
  MemFree(stateLast);
  MemFree(stateCurr);
}

void Keyboard_UpdatePre () {
  int size; uchar const* state = SDL_GetKeyboardState(&size);
  MemCpy(stateLast, state, size);
}

void Keyboard_UpdatePost () {
  int size; uchar const* state = SDL_GetKeyboardState(&size);
  MemCpy(stateCurr, state, size);

  for (int i = 0; i < size; ++i) {
    if (stateCurr[i] != stateLast[i]) {
      lastAction = SDL_GetPerformanceCounter();
      break;
    }
  }
}

bool Keyboard_Down (Key key) {
   return stateCurr[key] != 0;
}

bool Keyboard_Pressed (Key key) {
  return stateCurr[key] && !stateLast[key];
}

bool Keyboard_Released (Key key) {
  return !stateCurr[key] && stateLast[key];
}

double Keyboard_GetIdleTime () {
  uint64 now = SDL_GetPerformanceCounter();
  return (double)(now - lastAction) / (double)SDL_GetPerformanceFrequency();
}

bool KeyMod_Alt () {
  return stateCurr[SDL_SCANCODE_LALT] || stateCurr[SDL_SCANCODE_RALT];
}

bool KeyMod_Ctrl () {
  return stateCurr[SDL_SCANCODE_LCTRL] || stateCurr[SDL_SCANCODE_RCTRL];
}

bool KeyMod_Shift () {
  return stateCurr[SDL_SCANCODE_LSHIFT] || stateCurr[SDL_SCANCODE_RSHIFT];
}

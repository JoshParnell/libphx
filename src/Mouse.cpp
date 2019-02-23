#include "Mouse.h"
#include "SDL.h"
#include "Vec2.h"

int lastX;
int lastY;
uint32 lastState;

static uint64 lastAction;
static int scrollAmount;

void Mouse_Init () {
  lastState = SDL_GetMouseState(&lastX, &lastY);
  lastAction = SDL_GetPerformanceCounter();
  scrollAmount = 0;
}

void Mouse_Free () {
}

void Mouse_SetScroll (int amount) {
  scrollAmount = amount;
}

void Mouse_Update () {
  int lx = lastX, ly = lastY;
  uint32 state = lastState;
  lastState = SDL_GetMouseState(&lastX, &lastY);
  if (lx != lastX || ly != lastY || state != lastState)
    lastAction = SDL_GetPerformanceCounter();
  scrollAmount = 0;
}

void Mouse_GetDelta (Vec2i* out) {
  SDL_GetMouseState(&out->x, &out->y);
  out->x -= lastX;
  out->y -= lastY;
}

double Mouse_GetIdleTime () {
  uint64 now = SDL_GetPerformanceCounter();
  return (double)(now - lastAction) / (double)SDL_GetPerformanceFrequency();
}

void Mouse_GetPosition (Vec2i* out) {
  SDL_GetMouseState(&out->x, &out->y);
}

void Mouse_GetPositionGlobal (Vec2i* out) {
  SDL_GetGlobalMouseState(&out->x, &out->y);
}

int Mouse_GetScroll () {
  return scrollAmount;
}

void Mouse_SetPosition (int x, int y) {
  SDL_WarpMouseInWindow(0, x, y);
}

void Mouse_SetVisible (bool visible) {
  SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

bool Mouse_Down (MouseButton button) {
  button = SDL_BUTTON(button);
  return (SDL_GetMouseState(0, 0) & button) > 0;
}

bool Mouse_Pressed (MouseButton button) {
  button = SDL_BUTTON(button);
  uint32 current = SDL_GetMouseState(0, 0);
  return (current & button) && !(lastState & button);
}

bool Mouse_Released (MouseButton button) {
  button = SDL_BUTTON(button);
  uint32 current = SDL_GetMouseState(0, 0);
  return !(current & button) && (lastState & button);
}

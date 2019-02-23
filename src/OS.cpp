#include "OS.h"
#include "SDL.h"

cstr OS_GetClipboard () {
  return SDL_GetClipboardText();
}

int OS_GetCPUCount () {
  return SDL_GetCPUCount();
}

cstr OS_GetVideoDriver () {
  return SDL_GetCurrentVideoDriver();
}

void OS_SetClipboard (cstr text) {
  if (SDL_SetClipboardText(text) != 0)
    Fatal("OS_SetClipboard: %s", SDL_GetError());
}

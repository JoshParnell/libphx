#include "PhxMemory.h"
#include "OpenGL.h"
#include "SDL.h"
#include "Vec2.h"
#include "Viewport.h"
#include "Window.h"
#include "WindowMode.h"

struct Window {
  SDL_Window* handle;
  SDL_GLContext context;
  WindowMode mode;
};

Window* Window_Create (cstr title, int x, int y, int sx, int sy, WindowMode mode) {
  Window* self = MemNew(Window);
  mode |= SDL_WINDOW_OPENGL;
  self->handle = SDL_CreateWindow(title, x, y, sx, sy, mode);
  self->context = SDL_GL_CreateContext(self->handle);
  self->mode = mode;
  if (!self->context)
    Fatal("Failed to create OpenGL context for window");
  OpenGL_Init();
  return self;
}

void Window_Free (Window* self) {
  SDL_GL_DeleteContext(self->context);
  SDL_DestroyWindow(self->handle);
  MemFree(self);
}

void Window_BeginDraw (Window* self) {
  Vec2i size;
  SDL_GL_MakeCurrent(self->handle, self->context);
  Window_GetSize(self, &size);
  Viewport_Push(0, 0, size.x, size.y, true);
}

void Window_EndDraw (Window* self) {
  Viewport_Pop();
  SDL_GL_SwapWindow(self->handle);
}

void Window_GetSize (Window* self, Vec2i* out) {
  SDL_GetWindowSize(self->handle, &out->x, &out->y);
}

void Window_GetPosition (Window* self, Vec2i* out) {
  SDL_GetWindowPosition(self->handle, &out->x, &out->y);
}

cstr Window_GetTitle (Window* self) {
  return SDL_GetWindowTitle(self->handle);
}

void Window_SetFullscreen (Window* self, bool fs) {
  SDL_SetWindowFullscreen(self->handle, fs ? WindowMode_Fullscreen : 0);
}

void Window_SetPosition (Window* self, WindowPos x, WindowPos y) {
  SDL_SetWindowPosition(self->handle, x, y);
}

void Window_SetSize (Window* self, int sx, int sy) {
  SDL_SetWindowSize(self->handle, sx, sy);
}

void Window_SetTitle (Window* self, cstr title) {
  SDL_SetWindowTitle(self->handle, title);
}

void Window_SetVsync (Window*, bool vsync) {
  SDL_GL_SetSwapInterval(vsync ? 1 : 0);
}

void Window_ToggleFullscreen (Window* self) {
  if (self->mode & WindowMode_Fullscreen)
    SDL_SetWindowFullscreen(self->handle, 0);
  else
    SDL_SetWindowFullscreen(self->handle, WindowMode_Fullscreen);
  self->mode ^= WindowMode_Fullscreen;
}

void Window_Hide (Window* self) {
  SDL_HideWindow(self->handle);
}

void Window_Show (Window* self) {
  SDL_ShowWindow(self->handle);
}

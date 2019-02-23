#ifndef PHX_OpenGL
#define PHX_OpenGL

#include "Common.h"
#include "GL/glew.h"

void  OpenGL_Init       ();
void  OpenGL_CheckError (cstr file, int line);

/* WARNING - Calling glGetError between glBegin / glEnd IS ITSELF an error.
 *           Hence, only the glEnd call of an immediate-mode draw sequence
 *           should be wrapped in GLCALL. */
#if ENABLE_GLCHECK
  #define GLCALL(x) { x; OpenGL_CheckError(__FILE__, __LINE__); }
#else
  #define GLCALL(x) x;
#endif

#endif

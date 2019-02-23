#include "OpenGL.h"
#include "PixelFormat.h"

const PixelFormat PixelFormat_Red             = GL_RED;
const PixelFormat PixelFormat_RG              = GL_RG;
const PixelFormat PixelFormat_RGB             = GL_RGB;
const PixelFormat PixelFormat_BGR             = GL_BGR;
const PixelFormat PixelFormat_RGBA            = GL_RGBA;
const PixelFormat PixelFormat_BGRA            = GL_BGRA;
const PixelFormat PixelFormat_Depth_Component = GL_DEPTH_COMPONENT;

int PixelFormat_Components (PixelFormat self) {
  switch (self) {
    case PixelFormat_Red:
    case PixelFormat_Depth_Component:
      return 1;
    case PixelFormat_RG:
      return 2;
    case PixelFormat_RGB:
    case PixelFormat_BGR:
      return 3;
    case PixelFormat_RGBA:
    case PixelFormat_BGRA:
      return 4;
  }
  return 0;
}

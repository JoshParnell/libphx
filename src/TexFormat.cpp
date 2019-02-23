#include "OpenGL.h"
#include "TexFormat.h"

const TexFormat TexFormat_R8       = GL_R8;
const TexFormat TexFormat_R16      = GL_R16;
const TexFormat TexFormat_R16F     = GL_R16F;
const TexFormat TexFormat_R32F     = GL_R32F;
const TexFormat TexFormat_RG8      = GL_RG8;
const TexFormat TexFormat_RG16     = GL_RG16;
const TexFormat TexFormat_RG16F    = GL_RG16F;
const TexFormat TexFormat_RG32F    = GL_RG32F;
const TexFormat TexFormat_RGB8     = GL_RGB8;
const TexFormat TexFormat_RGBA8    = GL_RGBA8;
const TexFormat TexFormat_RGBA16   = GL_RGBA16;
const TexFormat TexFormat_RGBA16F  = GL_RGBA16F;
const TexFormat TexFormat_RGBA32F  = GL_RGBA32F;
const TexFormat TexFormat_Depth16  = GL_DEPTH_COMPONENT16;
const TexFormat TexFormat_Depth24  = GL_DEPTH_COMPONENT24;
const TexFormat TexFormat_Depth32F = GL_DEPTH_COMPONENT32F;

int TexFormat_Components (TexFormat self) {
  switch (self) {
    case TexFormat_R8:
    case TexFormat_R16:
    case TexFormat_R16F:
    case TexFormat_R32F:
    case TexFormat_Depth16:
    case TexFormat_Depth24:
    case TexFormat_Depth32F:
      return 1;
    case TexFormat_RG8:
    case TexFormat_RG16:
    case TexFormat_RG16F:
    case TexFormat_RG32F:
      return 2;
    case TexFormat_RGB8:
      return 3;
    case TexFormat_RGBA8:
    case TexFormat_RGBA16:
    case TexFormat_RGBA16F:
    case TexFormat_RGBA32F:
      return 4;
  }
  return 0;
}

int TexFormat_GetSize (TexFormat self) {
  switch (self) {
    case TexFormat_R8:
      return 1;
    case TexFormat_R16:
    case TexFormat_R16F:
    case TexFormat_RG8:
    case TexFormat_Depth16:
      return 2;
    case TexFormat_RGB8:
    case TexFormat_Depth24:
      return 3;
    case TexFormat_R32F:
    case TexFormat_RG16:
    case TexFormat_RG16F:
    case TexFormat_RGBA8:
    case TexFormat_Depth32F:
      return 4;
    case TexFormat_RG32F:
    case TexFormat_RGBA16:
    case TexFormat_RGBA16F:
      return 8;
    case TexFormat_RGBA32F:
      return 16;
  }
  return 0;
}

bool TexFormat_IsColor (TexFormat self) {
  switch (self) {
    case TexFormat_Depth16:
    case TexFormat_Depth24:
    case TexFormat_Depth32F:
      return false;
  }
  return true;
}

bool TexFormat_IsDepth (TexFormat self) {
  switch (self) {
    case TexFormat_Depth16:
    case TexFormat_Depth24:
    case TexFormat_Depth32F:
      return true;
  }
  return false;
}

bool TexFormat_IsValid (TexFormat self) {
  switch (self) {
    case TexFormat_R8:
    case TexFormat_R16:
    case TexFormat_R16F:
    case TexFormat_R32F:
    case TexFormat_RG8:
    case TexFormat_RG16:
    case TexFormat_RG16F:
    case TexFormat_RG32F:
    case TexFormat_RGB8:
    case TexFormat_RGBA8:
    case TexFormat_RGBA16:
    case TexFormat_RGBA16F:
    case TexFormat_RGBA32F:
    case TexFormat_Depth16:
    case TexFormat_Depth24:
    case TexFormat_Depth32F:
      return true;
  }
  return false;
}

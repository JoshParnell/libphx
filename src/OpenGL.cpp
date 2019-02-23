#include "BlendMode.h"
#include "CullFace.h"
#include "OpenGL.h"
#include "RenderState.h"

void OpenGL_Init () {
  static bool init = false;
  if (!init) {
    init = true;
    glewInit();
  }

  GLCALL(glDisable(GL_MULTISAMPLE))
  GLCALL(glDisable(GL_CULL_FACE));
  GLCALL(glCullFace(GL_BACK))

  GLCALL(glPixelStorei(GL_PACK_ALIGNMENT, 1))
  GLCALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1))
  GLCALL(glDepthFunc(GL_LEQUAL))

  GLCALL(glEnable(GL_BLEND))
  GLCALL(glBlendFunc(GL_ONE, GL_ZERO))

  GLCALL(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS))
  GLCALL(glDisable(GL_POINT_SMOOTH))
  GLCALL(glDisable(GL_LINE_SMOOTH))
  GLCALL(glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST))
  GLCALL(glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST))
  GLCALL(glLineWidth(2))

  GLCALL(glMatrixMode(GL_PROJECTION))
  GLCALL(glLoadIdentity())
  GLCALL(glMatrixMode(GL_MODELVIEW))
  GLCALL(glLoadIdentity())

  // GLCALL(glDepthRange(-1, 1))

  RenderState_PushAllDefaults();
}

void OpenGL_CheckError (cstr file, int line) {
  GLenum errorID = glGetError();
  cstr error = 0;
  switch (errorID) {
    case GL_NO_ERROR: return;
    case GL_INVALID_ENUM:
      error = "GL_INVALID_ENUM"; break;
    case GL_INVALID_VALUE:
      error = "GL_INVALID_VALUE"; break;
    case GL_INVALID_OPERATION:
      error = "GL_INVALID_OPERATION"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
    case GL_OUT_OF_MEMORY:
      error = "GL_OUT_OF_MEMORY"; break;
    default:
      Fatal("OpenGL_CheckError: glGetError returned illegal error code %u at %s:%d", errorID, file, line);
      break;
  }
  Fatal("OpenGL_CheckError: %s at %s:%d", error, file, line);
}

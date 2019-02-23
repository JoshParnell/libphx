#include "OpenGL.h"
#include "TexFilter.h"

const TexFilter TexFilter_Point           = GL_NEAREST;
const TexFilter TexFilter_PointMipPoint   = GL_NEAREST_MIPMAP_NEAREST;
const TexFilter TexFilter_PointMipLinear  = GL_NEAREST_MIPMAP_LINEAR;
const TexFilter TexFilter_Linear          = GL_LINEAR;
const TexFilter TexFilter_LinearMipPoint  = GL_LINEAR_MIPMAP_NEAREST;
const TexFilter TexFilter_LinearMipLinear = GL_LINEAR_MIPMAP_LINEAR;

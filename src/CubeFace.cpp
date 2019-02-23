#include "CubeFace.h"
#include "OpenGL.h"

const CubeFace CubeFace_PX = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
const CubeFace CubeFace_NX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
const CubeFace CubeFace_PY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
const CubeFace CubeFace_NY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
const CubeFace CubeFace_PZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
const CubeFace CubeFace_NZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

static CubeFace const kFaces[] = {
  CubeFace_PX,
  CubeFace_NX,
  CubeFace_PY,
  CubeFace_NY,
  CubeFace_PZ,
  CubeFace_NZ,
};

CubeFace CubeFace_Get (int index) {
  return kFaces[index];
}

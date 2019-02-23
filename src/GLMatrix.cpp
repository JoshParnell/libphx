#include "GLMatrix.h"
#include "Matrix.h"
#include "OpenGL.h"
#include "PhxMath.h"
#include "Vec3.h"

/* NOTE : LoadMatrix expects column-major memory layout, but we use row-major,
 *        hence the need for transpositions when taking a Matrix*. */

void GLMatrix_Clear () {
  GLCALL(glLoadIdentity())
}

void GLMatrix_Load (Matrix* matrix) {
  float* m = (float*)matrix;
  float transpose[] = {
    m[ 0], m[ 4], m[ 8], m[12],
    m[ 1], m[ 5], m[ 9], m[13],
    m[ 2], m[ 6], m[10], m[14],
    m[ 3], m[ 7], m[11], m[15],
  };
  GLCALL(glLoadMatrixf(transpose))
}

void GLMatrix_LookAt (Vec3d const* eye, Vec3d const* at, Vec3d const* up) {
  Vec3d z = Vec3d_Normalize(Vec3d_Sub(*at, *eye));
  Vec3d x = Vec3d_Normalize(Vec3d_Cross(z, Vec3d_Normalize(*up)));
  Vec3d y = Vec3d_Cross(x, z);

  /* TODO : Yet another sign flip. Sigh. */
  double m[16] = {
    x.x, y.x, -z.x, 0,
    x.y, y.y, -z.y, 0,
    x.z, y.z, -z.z, 0,
    0,     0,    0, 1,
  };

  GLCALL(glMultMatrixd(m))
  GLCALL(glTranslated(-eye->x, -eye->y, -eye->z))
}

void GLMatrix_ModeP () {
  GLCALL(glMatrixMode(GL_PROJECTION))
}

void GLMatrix_ModeWV () {
  GLCALL(glMatrixMode(GL_MODELVIEW))
}

void GLMatrix_Mult (Matrix* matrix) {
  float* m = (float*)matrix;
  float transpose[] = {
    m[ 0], m[ 4], m[ 8], m[12],
    m[ 1], m[ 5], m[ 9], m[13],
    m[ 2], m[ 6], m[10], m[14],
    m[ 3], m[ 7], m[11], m[15],
  };
  GLCALL(glMultMatrixf((float*)transpose))
}

void GLMatrix_Perspective (double fovy, double aspect, double z0, double z1) {
  double rads = Pi * fovy / 360.0;
  double cot = 1.0 / Tan(rads);
  double dz = z1 - z0;
  double nf = -2.0 * (z0 * z1) / dz;

  double m[16] = {
    cot / aspect,   0,               0,    0,
    0,            cot,               0,    0,
    0,              0, -(z0 + z1) / dz, -1.0,
    0,              0,              nf,    0,
  };

  GLCALL(glMultMatrixd(m))
}

void GLMatrix_Pop () {
  GLCALL(glPopMatrix())
}

void GLMatrix_Push () {
  GLCALL(glPushMatrix())
}

void GLMatrix_PushClear () {
  GLCALL(glPushMatrix())
  GLCALL(glLoadIdentity())
}

Matrix* GLMatrix_Get () {
  GLint matrixMode;
  GLCALL(glGetIntegerv(GL_MATRIX_MODE, &matrixMode));

  switch (matrixMode) {
    case GL_MODELVIEW:  matrixMode = GL_MODELVIEW_MATRIX;  break;
    case GL_PROJECTION: matrixMode = GL_PROJECTION_MATRIX; break;

    case GL_COLOR:
    case GL_TEXTURE:
    default: return 0;
  }

  Matrix* matrix = Matrix_Identity();
  GLCALL(glGetFloatv(matrixMode, (float*) matrix));
  return matrix;
}

void GLMatrix_RotateX (double angle) {
  GLCALL(glRotated(angle, 1, 0, 0))
}

void GLMatrix_RotateY (double angle) {
  GLCALL(glRotated(angle, 0, 1, 0))
}

void GLMatrix_RotateZ (double angle) {
  GLCALL(glRotated(angle, 0, 0, 1))
}

void GLMatrix_Scale (double x, double y, double z) {
  GLCALL(glScaled(x, y, z))
}

void GLMatrix_Translate (double x, double y, double z) {
  GLCALL(glTranslated(x, y, z))
}

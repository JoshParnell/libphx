#include "Box3.h"
#include "Matrix.h"
#include "MatrixDef.h"
#include "PhxMemory.h"
#include "Quat.h"
#include "PhxMath.h"
#include "Vec3.h"
#include "Vec4.h"

#include <stdio.h>

Matrix* Matrix_Clone (Matrix const* self) {
  Matrix* clone = MemNew(Matrix);
  *clone = *self;
  return clone;
}

void Matrix_Free (Matrix* self) {
  MemFree(self);
}

static void Matrix_IOInverse (Matrix const* NO_ALIAS in, Matrix* NO_ALIAS out) {
  float const* src = (float const*)in;
  float* dst       = (float*)out;

  dst[0] =
    src[ 5] * src[10] * src[15] - src[ 5] * src[11] * src[14] -
    src[ 9] * src[ 6] * src[15] + src[ 9] * src[ 7] * src[14] +
    src[13] * src[ 6] * src[11] - src[13] * src[ 7] * src[10];

  dst[4] =
   -src[ 4] * src[10] * src[15] + src[ 4] * src[11] * src[14] +
    src[ 8] * src[ 6] * src[15] - src[ 8] * src[ 7] * src[14] -
    src[12] * src[ 6] * src[11] + src[12] * src[ 7] * src[10];

  dst[8] =
    src[ 4] * src[ 9] * src[15] - src[ 4] * src[11] * src[13] -
    src[ 8] * src[ 5] * src[15] + src[ 8] * src[ 7] * src[13] +
    src[12] * src[ 5] * src[11] - src[12] * src[ 7] * src[9];

  dst[12] =
   -src[ 4] * src[ 9] * src[14] + src[ 4] * src[10] * src[13] +
    src[ 8] * src[ 5] * src[14] - src[ 8] * src[ 6] * src[13] -
    src[12] * src[ 5] * src[10] + src[12] * src[ 6] * src[ 9];

  dst[1] =
   -src[ 1] * src[10] * src[15] + src[ 1] * src[11] * src[14] +
    src[ 9] * src[ 2] * src[15] - src[ 9] * src[ 3] * src[14] -
    src[13] * src[ 2] * src[11] + src[13] * src[ 3] * src[10];

  dst[5] =
    src[ 0] * src[10] * src[15] - src[ 0] * src[11] * src[14] -
    src[ 8] * src[ 2] * src[15] + src[ 8] * src[ 3] * src[14] +
    src[12] * src[ 2] * src[11] - src[12] * src[ 3] * src[10];

  dst[9] =
   -src[ 0] * src[ 9] * src[15] + src[ 0] * src[11] * src[13] +
    src[ 8] * src[ 1] * src[15] - src[ 8] * src[ 3] * src[13] -
    src[12] * src[ 1] * src[11] + src[12] * src[ 3] * src[ 9];

  dst[13] =
    src[ 0] * src[ 9] * src[14] - src[ 0] * src[10] * src[13] -
    src[ 8] * src[ 1] * src[14] + src[ 8] * src[ 2] * src[13] +
    src[12] * src[ 1] * src[10] - src[12] * src[ 2] * src[ 9];

  dst[2] =
    src[ 1] * src[ 6] * src[15] - src[ 1] * src[ 7] * src[14] -
    src[ 5] * src[ 2] * src[15] + src[ 5] * src[ 3] * src[14] +
    src[13] * src[ 2] * src[ 7] - src[13] * src[ 3] * src[ 6];

  dst[6] =
   -src[ 0] * src[ 6] * src[15] + src[ 0] * src[ 7] * src[14] +
    src[ 4] * src[ 2] * src[15] - src[ 4] * src[ 3] * src[14] -
    src[12] * src[ 2] * src[ 7] + src[12] * src[ 3] * src[ 6];

  dst[10] =
    src[ 0] * src[ 5] * src[15] - src[ 0] * src[ 7] * src[13] -
    src[ 4] * src[ 1] * src[15] + src[ 4] * src[ 3] * src[13] +
    src[12] * src[ 1] * src[ 7] - src[12] * src[ 3] * src[ 5];

  dst[14] =
   -src[ 0] * src[ 5] * src[14] + src[ 0] * src[ 6] * src[13] +
    src[ 4] * src[ 1] * src[14] - src[ 4] * src[ 2] * src[13] -
    src[12] * src[ 1] * src[ 6] + src[12] * src[ 2] * src[ 5];

  dst[3] =
   -src[ 1] * src[ 6] * src[11] + src[ 1] * src[ 7] * src[10] +
    src[ 5] * src[ 2] * src[11] - src[ 5] * src[ 3] * src[10] -
    src[ 9] * src[ 2] * src[ 7] + src[ 9] * src[ 3] * src[ 6];

  dst[7] =
    src[ 0] * src[ 6] * src[11] - src[ 0] * src[ 7] * src[10] -
    src[ 4] * src[ 2] * src[11] + src[ 4] * src[ 3] * src[10] +
    src[ 8] * src[ 2] * src[ 7] - src[ 8] * src[ 3] * src[ 6];

  dst[11] =
   -src[ 0] * src[ 5] * src[11] + src[ 0] * src[ 7] * src[ 9] +
    src[ 4] * src[ 1] * src[11] - src[ 4] * src[ 3] * src[ 9] -
    src[ 8] * src[ 1] * src[ 7] + src[ 8] * src[ 3] * src[ 5];

  dst[15] =
    src[ 0] * src[ 5] * src[10] - src[ 0] * src[ 6] * src[ 9] -
    src[ 4] * src[ 1] * src[10] + src[ 4] * src[ 2] * src[ 9] +
    src[ 8] * src[ 1] * src[ 6] - src[ 8] * src[ 2] * src[ 5];

  float det = 1.0f /
    (src[0] * dst[0] + src[1] * dst[4] + src[2] * dst[8] + src[3] * dst[12]);
  for (int i = 0; i < 16; ++i)
    dst[i] *= det;
}

static void Matrix_IOTranspose (Matrix const* NO_ALIAS in, Matrix* NO_ALIAS out) {
  float const* src = (float const*)in;
  float* dst       = (float*)out;
  dst[ 0] = src[ 0]; dst[ 1] = src[ 4]; dst[ 2] = src[ 8]; dst[ 3] = src[12];
  dst[ 4] = src[ 1]; dst[ 5] = src[ 5]; dst[ 6] = src[ 9]; dst[ 7] = src[13];
  dst[ 8] = src[ 2]; dst[ 9] = src[ 6]; dst[10] = src[10]; dst[11] = src[14];
  dst[12] = src[ 3]; dst[13] = src[ 7]; dst[14] = src[11]; dst[15] = src[15];
}

bool Matrix_Equal (Matrix const* a, Matrix const* b) {
  for (int i = 0; i < 16; i++) {
    if (a->m[i] != b->m[i])
      return false;
  }
  return true;
}

bool Matrix_ApproximatelyEqual (Matrix const* a, Matrix const* b) {
  for (int i = 0; i < 16; i++) {
    if (!Float_ApproximatelyEqual(a->m[i], b->m[i]))
      return false;
  }
  return true;
}

Matrix* Matrix_Inverse (Matrix const* self) {
  Matrix result;
  Matrix_IOInverse(self, &result);
  return Matrix_Clone(&result);
}

Matrix* Matrix_InverseTranspose (Matrix const* self) {
  Matrix inverse, result;
  Matrix_IOInverse(self, &inverse);
  Matrix_IOTranspose(&inverse, &result);
  return Matrix_Clone(&result);
}

Matrix* Matrix_Sum (Matrix const* a, Matrix const* b) {
  Matrix result;
  for (int i = 0; i < 16; ++i) {
    result.m[i] = a->m[i] + b->m[i];
  }
  return Matrix_Clone(&result);
}

Matrix* Matrix_Transpose (Matrix const* self) {
  Matrix result;
  Matrix_IOTranspose(self, &result);
  return Matrix_Clone(&result);
}

void Matrix_IInverse (Matrix* self) {
  Matrix result;
  Matrix_IOInverse(self, &result);
  *self = result;
}

void Matrix_IScale (Matrix* self, float scale) {
  float* m = self->m;
  m[0] *= scale; m[1] *= scale; m[ 2] *= scale;
  m[4] *= scale; m[5] *= scale; m[ 6] *= scale;
  m[8] *= scale; m[9] *= scale; m[10] *= scale;
}

void Matrix_ITranspose (Matrix* self) {
  Matrix result;
  Matrix_IOTranspose(self, &result);
  *self = result;
}

Matrix* Matrix_Identity () {
  Matrix const identity = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
  };
  return Matrix_Clone(&identity);
}

Matrix* Matrix_LookAt (Vec3f const* pos, Vec3f const* at, Vec3f const* up) {
  Vec3f z = Vec3f_Normalize(Vec3f_Sub(*pos, *at));
  Vec3f x = Vec3f_Normalize(Vec3f_Cross(*up, z));
  Vec3f y = Vec3f_Cross(z, x);
  Matrix result = {
     x.x, y.x, z.x, pos->x,
     x.y, y.y, z.y, pos->y,
     x.z, y.z, z.z, pos->z,
       0,   0,   0,      1,
  };
  return Matrix_Clone(&result);
}

Matrix* Matrix_LookUp (Vec3f const* pos, Vec3f const* look, Vec3f const* up) {
  Vec3f z = Vec3f_Normalize(Vec3f_Muls(*look, -1.0f));
  Vec3f x = Vec3f_Normalize(Vec3f_Cross(*up, z));
  Vec3f y = Vec3f_Cross(z, x);
  Matrix result = {
     x.x, y.x, z.x, pos->x,
     x.y, y.y, z.y, pos->y,
     x.z, y.z, z.z, pos->z,
       0,   0,   0,      1,
  };
  return Matrix_Clone(&result);
}

Matrix* Matrix_Perspective (float degreesFovy, float aspect, float N, float F) {
  double rads = Pi * degreesFovy / 360.0;
  double cot = 1.0 / Tan(rads);
  Matrix result = {
    (float)(cot / aspect), 0, 0, 0,
    0, (float)cot, 0, 0,
    0, 0, (N + F) / (N - F), (float)(2.0 * (F * N) / (N - F)),
    0, 0, -1.0f, 0,
  };
  return Matrix_Clone(&result);
}

Matrix* Matrix_Product (Matrix const* a, Matrix const* b) {
  Matrix result;
  float* pResult = result.m;
  for (int i = 0; i < 4; ++i)
  for (int j = 0; j < 4; ++j) {
    float sum = 0.0f;
    for (int k = 0; k < 4; ++k)
      sum += a->m[4 * i + k] * b->m[4 * k + j];
    *pResult++ = sum;
  }
  return Matrix_Clone(&result);
}

Matrix* Matrix_RotationX (float rads) {
  float c = Cos(rads);
  float s = Sin(rads);
  Matrix result = {
    1,  0,  0,  0,
    0,  c, -s,  0,
    0,  s,  c,  0,
    0,  0,  0,  1,
  };
  return Matrix_Clone(&result);
}

Matrix* Matrix_RotationY (float rads) {
  float c = Cos(rads);
  float s = Sin(rads);
  Matrix result = {
    c,  0,  s,  0,
    0,  1,  0,  0,
   -s,  0,  c,  0,
    0,  0,  0,  1,
  };
  return Matrix_Clone(&result);
}

Matrix* Matrix_RotationZ (float rads) {
  float c = Cos(rads);
  float s = Sin(rads);
  Matrix result = {
    c, -s,  0,  0,
    s,  c,  0,  0,
    0,  0,  1,  0,
    0,  0,  0,  1,
  };
  return Matrix_Clone(&result);
}

Matrix* Matrix_Scaling (float sx, float sy, float sz) {
  Matrix result = {
    sx,  0,  0, 0,
     0, sy,  0, 0,
     0,  0, sz, 0,
     0,  0,  0, 1,
  };
  return Matrix_Clone(&result);
}

/* TODO : Be less lazy. */
Matrix* Matrix_SRT (
  float sx, float sy, float sz,
  float ry, float rp, float rr,
  float tx, float ty, float tz)
{
  Matrix* S = Matrix_Scaling(sx, sy, sz);
  Matrix* R = Matrix_YawPitchRoll(ry, rp, rr);
  Matrix* T = Matrix_Translation(tx, ty, tz);
  Matrix* TR = Matrix_Product(T, R);
  Matrix* TRS = Matrix_Product(TR, S);
  Matrix_Free(S);
  Matrix_Free(R);
  Matrix_Free(T);
  Matrix_Free(TR);
  return TRS;
}

Matrix* Matrix_Translation (float tx, float ty, float tz) {
  Matrix result = {
     1,  0,  0, tx,
     0,  1,  0, ty,
     0,  0,  1, tz,
     0,  0,  0,  1,
  };
  return Matrix_Clone(&result);
}

Matrix* Matrix_YawPitchRoll (float yaw, float pitch, float roll) {
  float ca = Cos(roll);
  float sa = Sin(roll);
  float cb = Cos(yaw);
  float sb = Sin(yaw);
  float cy = Cos(pitch);
  float sy = Sin(pitch);
  Matrix result = {
    ca * cb, ca * sb * sy - sa * cy, ca * sb * cy + sa * sy, 0,
    sa * cb, sa * sb * sy + ca * cy, sa * sb * cy - ca * sy, 0,
    -sb, cb * sy, cb * cy, 0,
    0, 0, 0, 1,
  };
  return Matrix_Clone(&result);
}

void Matrix_MulBox (Matrix const* self, Box3f* NO_ALIAS out, Box3f const* NO_ALIAS in) {
  Vec3f const corners[] = {
    { in->lower.x, in->lower.y, in->lower.z },
    { in->upper.x, in->lower.y, in->lower.z },
    { in->lower.x, in->upper.y, in->lower.z },
    { in->upper.x, in->upper.y, in->lower.z },
    { in->lower.x, in->lower.y, in->upper.z },
    { in->upper.x, in->lower.y, in->upper.z },
    { in->lower.x, in->upper.y, in->upper.z },
    { in->upper.x, in->upper.y, in->upper.z },
  };

  Vec3f result;
  Matrix_MulPoint(self, &result, corners[0].x, corners[0].y, corners[0].z);
  out->lower = result;
  out->upper = result;

  for (int i = 1; i < 8; ++i) {
    Matrix_MulPoint(self, &result, corners[i].x, corners[i].y, corners[i].z);
    out->lower = Vec3f_Min(out->lower, result);
    out->upper = Vec3f_Max(out->upper, result);
  }
}

void Matrix_MulDir (Matrix const* self, Vec3f* NO_ALIAS out, float x, float y, float z) {
  float const* m = self->m;
  out->x = m[ 0] * x + m[ 1] * y + m[ 2] * z;
  out->y = m[ 4] * x + m[ 5] * y + m[ 6] * z;
  out->z = m[ 8] * x + m[ 9] * y + m[10] * z;
}

void Matrix_MulPoint (Matrix const* self, Vec3f* NO_ALIAS out, float x, float y, float z) {
  float const* m = self->m;
  out->x = m[ 0] * x + m[ 1] * y + m[ 2] * z + m[ 3];
  out->y = m[ 4] * x + m[ 5] * y + m[ 6] * z + m[ 7];
  out->z = m[ 8] * x + m[ 9] * y + m[10] * z + m[11];
}

void Matrix_MulVec (Matrix const* self, Vec4f* NO_ALIAS out, float x, float y, float z, float w) {
  float const* m = self->m;
  out->x = m[ 0] * x + m[ 1] * y + m[ 2] * z + m[ 3] * w;
  out->y = m[ 4] * x + m[ 5] * y + m[ 6] * z + m[ 7] * w;
  out->z = m[ 8] * x + m[ 9] * y + m[10] * z + m[11] * w;
  out->w = m[12] * x + m[13] * y + m[14] * z + m[15] * w;
}

void Matrix_GetForward (Matrix const* self, Vec3f* out) {
  out->x = -self->m[2];
  out->y = -self->m[6];
  out->z = -self->m[10];
}

void Matrix_GetRight (Matrix const* self, Vec3f* out) {
  out->x = self->m[0];
  out->y = self->m[4];
  out->z = self->m[8];
}

void Matrix_GetUp (Matrix const* self, Vec3f* out) {
  out->x = self->m[1];
  out->y = self->m[5];
  out->z = self->m[9];
}

void Matrix_GetPos (Matrix const* self, Vec3f* out) {
  out->x = self->m[3];
  out->y = self->m[7];
  out->z = self->m[11];
}

void Matrix_GetRow (Matrix const* self, Vec4f* out, int row) {
  out->x = self->m[4 * row + 0];
  out->y = self->m[4 * row + 1];
  out->z = self->m[4 * row + 2];
  out->w = self->m[4 * row + 3];
}

Matrix* Matrix_FromBasis (Vec3f const* x, Vec3f const* y, Vec3f const* z) {
  Matrix result = {
    x->x, y->x, z->x, 0,
    x->y, y->y, z->y, 0,
    x->z, y->z, z->z, 0,
      0,   0,   0,    1
  };
  return Matrix_Clone(&result);
}

Matrix* Matrix_FromPosRot (Vec3f const* pos, Quat const* rot) {
  Vec3f x; Quat_GetAxisX(rot, &x);
  Vec3f y; Quat_GetAxisY(rot, &y);
  Vec3f z; Quat_GetAxisZ(rot, &z);
  Matrix result = {
    x.x, y.x, z.x, pos->x,
    x.y, y.y, z.y, pos->y,
    x.z, y.z, z.z, pos->z,
      0,   0,   0,      1
  };
  return Matrix_Clone(&result);
}

Matrix* Matrix_FromPosRotScale (Vec3f const* pos, Quat const* rot, float scale) {
  Vec3f x; Quat_GetAxisX(rot, &x);
  Vec3f y; Quat_GetAxisY(rot, &y);
  Vec3f z; Quat_GetAxisZ(rot, &z);
  Matrix result = {
    scale*x.x, scale*y.x, scale*z.x, pos->x,
    scale*x.y, scale*y.y, scale*z.y, pos->y,
    scale*x.z, scale*y.z, scale*z.z, pos->z,
            0,         0,         0,      1
  };
  return Matrix_Clone(&result);
}

Matrix* Matrix_FromPosBasis (Vec3f const* pos, Vec3f const* x, Vec3f const* y, Vec3f const* z) {
  Matrix result = {
    x->x, y->x, z->x, pos->x,
    x->y, y->y, z->y, pos->y,
    x->z, y->z, z->z, pos->z,
       0,    0,    0,      1
  };
  return Matrix_Clone(&result);
}

Matrix* Matrix_FromQuat (Quat const* q) {
  Vec3f x; Quat_GetAxisX(q, &x);
  Vec3f y; Quat_GetAxisY(q, &y);
  Vec3f z; Quat_GetAxisZ(q, &z);
  Matrix result = {
    x.x, y.x, z.x, 0,
    x.y, y.y, z.y, 0,
    x.z, y.z, z.z, 0,
      0,   0,   0, 1
  };
  return Matrix_Clone(&result);
}

void Matrix_ToQuat (Matrix const* self, Quat* q) {
  float const* m = (float const*) self;
  Vec3f x = { m[0], m[4], m[ 8] };
  Vec3f y = { m[1], m[5], m[ 9] };
  Vec3f z = { m[2], m[6], m[10] };
  Quat_FromBasis(&x, &y, &z, q);
}

void Matrix_Print (Matrix const* self) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j)
      printf("%f ", self->m[4 * i + j]);
    printf("\n");
  }
}

cstr Matrix_ToString (Matrix const* self) {
  static char buffer[512];

  const float* m = self->m;
  snprintf(buffer, (size_t) Array_GetSize(buffer),
      "[%+.2f, %+.2f, %+.2f, %+.2f]"
    "\n[%+.2f, %+.2f, %+.2f, %+.2f]"
    "\n[%+.2f, %+.2f, %+.2f, %+.2f]"
    "\n[%+.2f, %+.2f, %+.2f, %+.2f]",
    m[ 0], m[ 1], m[ 2], m[ 3],
    m[ 4], m[ 5], m[ 6], m[ 7],
    m[ 8], m[ 9], m[10], m[11],
    m[12], m[13], m[14], m[15]
  );
  return buffer;
}

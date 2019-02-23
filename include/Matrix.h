#ifndef PHX_Matrix
#define PHX_Matrix

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *   Matrices are stored with ROW-MAJOR memory layout:
 *
 *                                 float mat[] = {
 *     |  m0  m1  m2  m3 |           m0,  m1,  m2,  m3,          | Xx Yx Zx Tx |
 *     |  m4  m5  m6  m7 |  /---\    m4,  m5,  m6,  m7,   /---\  | Xy Yy Zy Ty |
 *     |  m8  m9 m10 m11 |  \---/    m8,  m9, m10, m11,   \---/  | Xz Yz Zz Tz |
 *     | m12 m13 m14 m15 |          m12, m13, m14, m15 };        |  0  0  0  1 |
 *
 *   We use POST-multiplication for matrices:
 *     Matrix_Product(A, B) = AB
 *
 *   And POST-multiplication for vectors (i.e., vectors are COLUMN vectors):
 *     Matrix_MulVec(A, v) = Av
 *
 *     |  A0   A1   A2   A3 |    | x |       |  A0*x +  A1*y +  A2*z +  A3*w |
 *     |  A4   A5   A6   A7 | \/ | y |  --\  |  A4*x +  A5*y +  A6*z +  A7*w |
 *     |  A8   A9  A10  A11 | /\ | z |  --/  |  A8*x +  A9*y + A10*z + A11*w |
 *     | A12  A13  A14  A15 |    | w |       | A12*x + A13*y + A14*z + A15*w |
 *
 *   Hence, Matrix_Product(A, Matrix_Product(B, C)) creates a matrix that
 *   applies transforms C, then B, then A, respectively, to incoming vectors.
 *
 *   Note that our storage order is transposed from OpenGL. In OpenGL bases are
 *   contiguous and translation is stored in 12, 13, 14.
 *
 * -------------------------------------------------------------------------- */

PHX_API Matrix*  Matrix_Clone               (Matrix const*);
PHX_API void     Matrix_Free                (Matrix*);

PHX_API bool     Matrix_Equal               (Matrix const*, Matrix const*);
PHX_API bool     Matrix_ApproximatelyEqual  (Matrix const*, Matrix const*);
PHX_API Matrix*  Matrix_Inverse             (Matrix const*);
PHX_API Matrix*  Matrix_InverseTranspose    (Matrix const*);
PHX_API Matrix*  Matrix_Product             (Matrix const*, Matrix const*);
PHX_API Matrix*  Matrix_Sum                 (Matrix const*, Matrix const*);
PHX_API Matrix*  Matrix_Transpose           (Matrix const*);

PHX_API void     Matrix_IInverse            (Matrix*);
PHX_API void     Matrix_IScale              (Matrix*, float);
PHX_API void     Matrix_ITranspose          (Matrix*);

PHX_API Matrix*  Matrix_Identity            ();
PHX_API Matrix*  Matrix_LookAt              (Vec3f const* pos, Vec3f const* at, Vec3f const* up);
PHX_API Matrix*  Matrix_LookUp              (Vec3f const* pos, Vec3f const* look, Vec3f const* up);
PHX_API Matrix*  Matrix_Perspective         (float degreesFovY, float aspect, float zNear, float zFar);
PHX_API Matrix*  Matrix_RotationX           (float rads);
PHX_API Matrix*  Matrix_RotationY           (float rads);
PHX_API Matrix*  Matrix_RotationZ           (float rads);
PHX_API Matrix*  Matrix_Scaling             (float sx, float sy, float sz);
PHX_API Matrix*  Matrix_SRT                 (float sx, float sy, float sz,
                                             float ry, float rp, float rr,
                                             float tx, float ty, float tz);
PHX_API Matrix*  Matrix_Translation         (float tx, float ty, float tz);
PHX_API Matrix*  Matrix_YawPitchRoll        (float yaw, float pitch, float roll);

PHX_API void     Matrix_MulBox              (Matrix const*, Box3f* out, Box3f const* in);
PHX_API void     Matrix_MulDir              (Matrix const*, Vec3f* out, float x, float y, float z);
PHX_API void     Matrix_MulPoint            (Matrix const*, Vec3f* out, float x, float y, float z);
PHX_API void     Matrix_MulVec              (Matrix const*, Vec4f* out, float x, float y, float z, float w);

PHX_API void     Matrix_GetForward          (Matrix const*, Vec3f* out);
PHX_API void     Matrix_GetRight            (Matrix const*, Vec3f* out);
PHX_API void     Matrix_GetUp               (Matrix const*, Vec3f* out);
PHX_API void     Matrix_GetPos              (Matrix const*, Vec3f* out);
PHX_API void     Matrix_GetRow              (Matrix const*, Vec4f* out, int row);

PHX_API Matrix*  Matrix_FromBasis           (Vec3f const* x, Vec3f const* y, Vec3f const* z);
PHX_API Matrix*  Matrix_FromPosRot          (Vec3f const*, Quat const*);
PHX_API Matrix*  Matrix_FromPosRotScale     (Vec3f const*, Quat const*, float);
PHX_API Matrix*  Matrix_FromPosBasis        (Vec3f const* pos, Vec3f const* x, Vec3f const* y, Vec3f const* z);
PHX_API Matrix*  Matrix_FromQuat            (Quat const*);
PHX_API void     Matrix_ToQuat              (Matrix const*, Quat* out);

PHX_API void     Matrix_Print               (Matrix const*);
PHX_API cstr     Matrix_ToString            (Matrix const*);

#endif

/* TODO : Is there any real reason for having scalar functions anymore? */

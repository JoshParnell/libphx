#ifndef PHX_GLMatrix
#define PHX_GLMatrix

#include "Common.h"
#include "Vec3.h"

PHX_API void     GLMatrix_Clear        ();
PHX_API void     GLMatrix_LookAt       (Vec3d const* eye, Vec3d const* at, Vec3d const* up);
PHX_API void     GLMatrix_Load         (Matrix* matrix);
PHX_API void     GLMatrix_ModeP        ();
PHX_API void     GLMatrix_ModeWV       ();
PHX_API Matrix*  GLMatrix_Get          ();
PHX_API void     GLMatrix_Mult         (Matrix* matrix);
PHX_API void     GLMatrix_Perspective  (double degreesFovY, double aspect, double zNear, double zFar);
PHX_API void     GLMatrix_Pop          ();
PHX_API void     GLMatrix_Push         ();
PHX_API void     GLMatrix_PushClear    ();
PHX_API void     GLMatrix_RotateX      (double rads);
PHX_API void     GLMatrix_RotateY      (double rads);
PHX_API void     GLMatrix_RotateZ      (double rads);
PHX_API void     GLMatrix_Scale        (double x, double y, double z);
PHX_API void     GLMatrix_Translate    (double x, double y, double z);

#endif

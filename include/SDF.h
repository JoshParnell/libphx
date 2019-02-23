#ifndef PHX_SDF
#define PHX_SDF

#include "Common.h"

PHX_API SDF*   SDF_Create          (int sx, int sy, int sz);
PHX_API SDF*   SDF_FromTex3D       (Tex3D*);
PHX_API void   SDF_Free            (SDF*);
PHX_API Mesh*  SDF_ToMesh          (SDF*);

PHX_API void   SDF_Clear           (SDF*, float value);
PHX_API void   SDF_ComputeNormals  (SDF*);
PHX_API void   SDF_Set             (SDF*, int x, int y, int z, float value);
PHX_API void   SDF_SetNormal       (SDF*, int x, int y, int z, Vec3f const* normal);

#endif

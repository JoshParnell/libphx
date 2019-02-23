#ifndef PHX_BoxMesh
#define PHX_BoxMesh

#include "Common.h"

PHX_API BoxMesh*  BoxMesh_Create   ();
PHX_API void      BoxMesh_Free     (BoxMesh*);

PHX_API void      BoxMesh_Add      (BoxMesh*, float px, float py, float pz,
                                              float sx, float sy, float sz,
                                              float rx, float ry, float rz,
                                              float bx, float by, float bz);
PHX_API Mesh*     BoxMesh_GetMesh  (BoxMesh*, int res);

#endif

#ifndef PHX_Meshes
#define PHX_Meshes

#include "Common.h"
#include "Vec3.h"

PHX_API Mesh*  Mesh_Box        (int res);
PHX_API Mesh*  Mesh_BoxSphere  (int res);
PHX_API Mesh*  Mesh_Plane      (Vec3f origin, Vec3f du, Vec3f dv, int resU, int resV);

#endif

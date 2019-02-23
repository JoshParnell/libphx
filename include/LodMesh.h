#ifndef PHX_LodMesh
#define PHX_LodMesh

#include "Common.h"

/* --- LodMesh -----------------------------------------------------------------
 *
 *   A basic container for abstracting LOD rendering behavior. LodMesh consists
 *   of any number of (Mesh, distMin, distMax) tuples. Drawing a requires
 *   passing a *distance squared* argument that is used to determine which
 *   component(s) of the LodMesh to draw.
 *
 *   This type is REFERENCE-COUNTED. See ../doc/RefCounted.txt for details.
 *
 * -------------------------------------------------------------------------- */

PHX_API LodMesh*  LodMesh_Create   ();
PHX_API void      LodMesh_Acquire  (LodMesh*);
PHX_API void      LodMesh_Free     (LodMesh*);

PHX_API void      LodMesh_Add      (LodMesh*, Mesh*, float distMin, float distMax);
PHX_API void      LodMesh_Draw     (LodMesh*, float distanceSquared);
PHX_API Mesh*     LodMesh_Get      (LodMesh*, float distanceSquared);

#endif

#ifndef PHX_Mesh
#define PHX_Mesh

#include "Common.h"

/* --- Mesh --------------------------------------------------------------------
 *
 *   This type is REFERENCE-COUNTED. See ../doc/RefCounted.txt for details.
 *   This type is VERSIONED. See ../doc/Versioned.txt for details.
 *
 *   The draw functions allow explicit binding and unbinding of a mesh's
 *   internal buffers. Explicit binding should be used when the same mesh must
 *   be drawn many times. It is unnecessary when a mesh will be drawn only once.
 *
 *     Mesh_DrawBind   : Binds internal buffers
 *     Mesh_DrawBound  : Assumes buffers have already been bound with DrawBind
 *     Mesh_DrawUnbind : Unbinds internal buffers; must be paired with DrawBind
 *                       call after draw operations are finished
 *     Mesh_Draw       : Equivalent to DrawBind -> DrawBound -> DrawUnbind
 *
 *   The following informational functions are computed lazily and cached
 *   according to the mesh version. Keeping an external cache is therefore
 *   redundant.
 *
 *     Mesh_GetBound  : Returns local AABB.
 *     Mesh_GetCenter : Returns the center of the mesh's AABB.
 *                      NOTE : In general, NOT equivalent to the centroid of
 *                      vertex positions! Equivalent to getting the center of
 *                      Mesh_GetBound.
 *     Mesh_GetRadius : Returns the radius of the minimal bounding sphere
 *                      with origin at the center of the local AABB.
 *                      NOTE : In general, NOT equivalent to minimal bounding
 *                             sphere radius; although usually a very good
 *                             approximation.
 *
 * -------------------------------------------------------------------------- */

PHX_API Mesh*    Mesh_Create             ();
PHX_API void     Mesh_Acquire            (Mesh*);
PHX_API void     Mesh_Free               (Mesh*);

PHX_API Mesh*    Mesh_Load               (cstr name);
PHX_API Mesh*    Mesh_Clone              (Mesh*);
PHX_API Bytes*   Mesh_ToBytes            (Mesh*);
PHX_API Mesh*    Mesh_FromBytes          (Bytes*);
PHX_API Mesh*    Mesh_FromObj            (cstr);
PHX_API Mesh*    Mesh_FromSDF            (SDF*);

PHX_API void     Mesh_AddIndex           (Mesh*, int);
PHX_API void     Mesh_AddMesh            (Mesh*, Mesh*);
PHX_API void     Mesh_AddQuad            (Mesh*, int, int, int, int);
PHX_API void     Mesh_AddTri             (Mesh*, int, int, int);
PHX_API void     Mesh_AddVertex          (Mesh*, float px, float py, float pz,
                                                 float nx, float ny, float nz,
                                                 float u, float v);
PHX_API void     Mesh_AddVertexRaw       (Mesh*, Vertex const*);

PHX_API uint64   Mesh_GetVersion         (Mesh*);
PHX_API void     Mesh_IncVersion         (Mesh*);

PHX_API void     Mesh_GetBound           (Mesh*, Box3f* out);
PHX_API void     Mesh_GetCenter          (Mesh*, Vec3f* out);
PHX_API int      Mesh_GetIndexCount      (Mesh*);
PHX_API int*     Mesh_GetIndexData       (Mesh*);
PHX_API float    Mesh_GetRadius          (Mesh*);
PHX_API Vertex*  Mesh_GetVertex          (Mesh*, int);
PHX_API int      Mesh_GetVertexCount     (Mesh*);
PHX_API Vertex*  Mesh_GetVertexData      (Mesh*);
PHX_API void     Mesh_ReserveIndexData   (Mesh*, int capacity);
PHX_API void     Mesh_ReserveVertexData  (Mesh*, int capacity);
PHX_API Error    Mesh_Validate           (Mesh*);

PHX_API void     Mesh_Draw               (Mesh*);
PHX_API void     Mesh_DrawBind           (Mesh*);
PHX_API void     Mesh_DrawBound          (Mesh*);
PHX_API void     Mesh_DrawUnbind         (Mesh*);
PHX_API void     Mesh_DrawNormals        (Mesh*, float scale);

PHX_API Mesh*    Mesh_Center             (Mesh*);
PHX_API Mesh*    Mesh_Invert             (Mesh*);
PHX_API Mesh*    Mesh_RotateX            (Mesh*, float rads);
PHX_API Mesh*    Mesh_RotateY            (Mesh*, float rads);
PHX_API Mesh*    Mesh_RotateZ            (Mesh*, float rads);
PHX_API Mesh*    Mesh_RotateYPR          (Mesh*, float yaw, float pitch, float roll);
PHX_API Mesh*    Mesh_Scale              (Mesh*, float x, float y, float z);
PHX_API Mesh*    Mesh_ScaleUniform       (Mesh*, float);
PHX_API Mesh*    Mesh_Transform          (Mesh*, Matrix*);
PHX_API Mesh*    Mesh_Translate          (Mesh*, float x, float y, float z);

PHX_API void     Mesh_ComputeAO          (Mesh*, float radius);
PHX_API void     Mesh_ComputeOcclusion   (Mesh*, Tex3D* sdf, float radius);
PHX_API void     Mesh_ComputeNormals     (Mesh*);
PHX_API void     Mesh_SplitNormals       (Mesh*, float minDot);

#endif

/* TODO : Mesh_LoadObj */

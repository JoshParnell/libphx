#include "Mesh.h"
#include "Meshes.h"
#include "Vertex.h"

inline static void Mesh_AddPlane (
  Mesh* self,
  Vec3f origin,
  Vec3f du, Vec3f dv,
  int resU, int resV)
{
  Vec3f n = Vec3f_Normalize(Vec3f_Cross(du, dv));

  for (int iu = 0; iu < resU; ++iu) {
    float u = (float)iu / (float)(resU - 1);
    for (int iv = 0; iv < resV; ++iv) {
      float v = (float)iv / (float)(resV - 1);
      Vec3f p = Vec3f_Add(origin,
        Vec3f_Add(
          Vec3f_Muls(du, u),
          Vec3f_Muls(dv, v)));

      if (iu && iv) {
        int vc = Mesh_GetVertexCount(self);
        Mesh_AddQuad(self, vc, vc - resV, vc - resV - 1, vc - 1);
      }

      Mesh_AddVertex(self, p.x, p.y, p.z, n.x, n.y, n.z, u, v);
    }
  }
}

Mesh* Mesh_Box (int res) {
  const Vec3f origin[6] = {
    { -1, -1,  1 }, { -1, -1, -1 },
    {  1, -1, -1 }, { -1, -1, -1 },
    { -1,  1, -1 }, { -1, -1, -1 },
  };

  const Vec3f du[6] = {
    { 2, 0, 0 }, { 0, 2, 0 }, { 0, 2, 0 }, { 0, 0, 2 }, { 0, 0, 2 }, { 2, 0, 0 },
  };

  const Vec3f dv[6] = {
    { 0, 2, 0 }, { 2, 0, 0 }, { 0, 0, 2 }, { 0, 2, 0 }, { 2, 0, 0 }, { 0, 0, 2 },
  };

  Mesh* self = Mesh_Create();
  for (int i = 0; i < 6; ++i)
    Mesh_AddPlane(self, origin[i], du[i], dv[i], res, res);
  return self;
}

Mesh* Mesh_BoxSphere (int res) {
  Mesh* self = Mesh_Box(res);
  int vertexCount = Mesh_GetVertexCount(self);
  Vertex* vertexData = Mesh_GetVertexData(self);
  for (int i = 0; i < vertexCount; ++i) {
    Vertex* vertex = vertexData + i;
    vertex->p = Vec3f_Normalize(vertex->p);
  }
  return self;
}

Mesh* Mesh_Plane (Vec3f origin, Vec3f du, Vec3f dv, int resU, int resV) {
  Mesh* self = Mesh_Create();
  Mesh_AddPlane(self, origin, du, dv, resU, resV);
  return self;
}

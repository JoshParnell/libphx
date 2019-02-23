#include "ArrayList.h"
#include "BoxMesh.h"
#include "Matrix.h"
#include "Mesh.h"
#include "Vec3.h"

struct Box {
  Vec3f p, s, r, b;
};

struct BoxMesh {
  ArrayList(Box, elem);
};

static Vec3f kFaceOrigin[] = {
  { -1, -1,  1 }, { -1, -1, -1 }, {  1, -1, -1 },
  { -1, -1, -1 }, { -1,  1, -1 }, { -1, -1, -1 },
};

static Vec3f kFaceU[] = {
  { 2, 0, 0 }, { 0, 2, 0 }, { 0, 2, 0 },
  { 0, 0, 2 }, { 0, 0, 2 }, { 2, 0, 0 },
};

static Vec3f kFaceV[] = {
  { 0, 2, 0 }, { 2, 0, 0 }, { 0, 0, 2 },
  { 0, 2, 0 }, { 2, 0, 0 }, { 0, 0, 2 },
};

BoxMesh* BoxMesh_Create () {
  BoxMesh* self = MemNew(BoxMesh);
  ArrayList_Init(self->elem);
  return self;
}

void BoxMesh_Free (BoxMesh* self) {
  ArrayList_Free(self->elem);
  MemFree(self);
}

void BoxMesh_Add (BoxMesh* self,
  float px, float py, float pz,
  float sx, float sy, float sz,
  float rx, float ry, float rz,
  float bx, float by, float bz)
{
  ArrayList_Grow(self->elem);
  Box* box = self->elem_data + (self->elem_size++);
  box->p = Vec3f_Create(px, py, pz);
  box->s = Vec3f_Create(sx, sy, sz);
  box->r = Vec3f_Create(rx, ry, rz);
  box->b = Vec3f_Create(bx, by, bz);
}

Mesh* BoxMesh_GetMesh (BoxMesh* self, int res) {
  Mesh* mesh = Mesh_Create();
  Mesh_ReserveVertexData(mesh, 6 * res * res * self->elem_size);
  Mesh_ReserveIndexData(mesh, 12 * (res - 1) * (res - 1));

  for (int i = 0; i < self->elem_size; ++i) {
    Box* box = self->elem_data + i;
    Vec3f lower = Vec3f_Create(box->b.x - 1.0f, box->b.y - 1.0f, box->b.z - 1.0f);
    Vec3f upper = Vec3f_Create(1.0f - box->b.x, 1.0f - box->b.y, 1.0f - box->b.z);
    Matrix* rot = Matrix_YawPitchRoll(box->r.x, box->r.y, box->r.z);

    for (int face = 0; face < 6; ++face) {
      Vec3f o = kFaceOrigin[face];
      Vec3f du = kFaceU[face];
      Vec3f dv = kFaceV[face];
      Vec3f n = Vec3f_Normalize(Vec3f_Cross(du, dv));

      for (int iu = 0; iu < res; ++iu) {
        float u = (float)iu / (float)(res - 1);
        for (int iv = 0; iv < res; ++iv) {
          float v = (float)iv / (float)(res - 1);
          Vec3f p = Vec3f_Add(o, Vec3f_Add(Vec3f_Muls(du, u), Vec3f_Muls(dv, v)));
          Vec3f clamped = Vec3f_Clamp(p, lower, upper);
          Vec3f proj = Vec3f_Sub(p, clamped);
          p = Vec3f_Add(clamped, Vec3f_Mul(Vec3f_SNormalize(proj), box->b));
          p = Vec3f_Mul(p, box->s);
          Vec3f rp;
          Matrix_MulPoint(rot, &rp, p.x, p.y, p.z);
          p = Vec3f_Add(rp, box->p);

          if (iu && iv) {
            int off = Mesh_GetVertexCount(mesh);
            Mesh_AddQuad(mesh, off, off - res, off - res - 1, off - 1);
          }
          Mesh_AddVertex(mesh, p.x, p.y, p.z, n.x, n.y, n.z, u, v);
        }
      }
    }

    Matrix_Free(rot);
  }

  return mesh;
}

#include "DataFormat.h"
#include "PhxMemory.h"
#include "Mesh.h"
#include "PixelFormat.h"
#include "SDF.h"
#include "Tex3D.h"
#include "Vec3.h"

struct Cell {
  float value;
  Vec3f normal;
};

struct SDF {
  Vec3i size;
  Cell* data;
};

SDF* SDF_Create (int sx, int sy, int sz) {
  SDF* self = MemNew(SDF);
  self->size = Vec3i_Create(sx, sy, sz);
  self->data = MemNewArray(Cell, sx * sy * sz);
  MemZero(self->data, sizeof(Cell) * sx * sy * sz);
  return self;
}

SDF* SDF_FromTex3D (Tex3D* tex) {
  SDF* self = MemNew(SDF);
  Tex3D_GetSize(tex, &self->size);
  self->data = MemNewArray(Cell, self->size.x * self->size.y * self->size.z);
  Tex3D_GetData(tex, self->data, PixelFormat_RGBA, DataFormat_Float);
  return self;
}

void SDF_Free (SDF* self) {
  MemFree(self->data);
  MemFree(self);
}

/* Triangulate using Surface Nets. */
Mesh* SDF_ToMesh (SDF* self) {
  Mesh* mesh = Mesh_Create();
  Vec3i const cells =  { self->size.x - 1, self->size.y - 1, self->size.z - 1 };
  Vec3f const cellsF = { (float)cells.x, (float)cells.y, (float)cells.z };
  Vec3i const stride = { 1, self->size.x, self->size.x * self->size.y };
  Vec3i const cellStride = { 1, cells.x, cells.x * cells.y };
  int* indices = MemNewArray(int, cells.x * cells.y * cells.z);

  Vec3f const vp[8] = {
    { 0, 0, 0 }, { 1, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 },
    { 0, 0, 1 }, { 1, 0, 1 }, { 0, 1, 1 }, { 1, 1, 1 },
  };

  int const edgeTable[12][2] = {
    { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 },
    { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
    { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
  };

  for (int z = 0; z < cells.z; ++z) {
    float z0 = (float)z / (float)cells.z;

    for (int y = 0; y < cells.y; ++y) {
      float y0 = (float)y / (float)cells.y;

      for (int x = 0; x < cells.x; ++x) {
        float x0 = (float)x / (float)cells.x;
        Vec3i cell = { x, y, z };
        int cellIndex = Vec3i_Dots(cellStride, x, y, z);

        Cell const* base = self->data + Vec3i_Dots(stride, x, y, z);
        Cell const* v[8] = {
          base,
          base + stride.x,
          base + stride.y,
          base + stride.x + stride.y,
          base + stride.z,
          base + stride.z + stride.x,
          base + stride.z + stride.y,
          base + stride.z + stride.y + stride.x,
        };

        int mask = 0;
        mask |= v[0]->value > 0 ? 0x01 : 0x00;
        mask |= v[1]->value > 0 ? 0x02 : 0x00;
        mask |= v[2]->value > 0 ? 0x04 : 0x00;
        mask |= v[3]->value > 0 ? 0x08 : 0x00;
        mask |= v[4]->value > 0 ? 0x10 : 0x00;
        mask |= v[5]->value > 0 ? 0x20 : 0x00;
        mask |= v[6]->value > 0 ? 0x40 : 0x00;
        mask |= v[7]->value > 0 ? 0x80 : 0x00;

        if (mask == 0x00 || mask == 0xFF) {
          indices[cellIndex] = -1;
          continue;
        }

        float tw = 0.0f;
        Vec3f offset = { 0, 0, 0 };
        Vec3f n = { 0, 0, 0 };

        /* Generate vertex. */
        for (int i = 0; i < 12; ++i) {
          int i0 = edgeTable[i][0];
          int i1 = edgeTable[i][1];
          Cell const* v0 = v[i0];
          Cell const* v1 = v[i1];
          if ((v0->value > 0) == (v1->value > 0))
            continue;

          float t = Saturate(v0->value / (v0->value - v1->value));
          Vec3f_IAdd(&offset, Vec3f_Lerp(vp[i0], vp[i1], t));
          Vec3f_IAdd(&n, Vec3f_Lerp(v0->normal, v1->normal, t));
          tw += 1.0f;
        }

        Vec3f_IDivs(&offset, tw);
        n = Vec3f_SNormalize(n);
        Vec3f p = Vec3f_Add(Vec3f_Create(x0, y0, z0), Vec3f_Div(offset, cellsF));
        p = Vec3f_Subs(Vec3f_Muls(p, 2.0f), 1.0f);

        indices[cellIndex] = Mesh_GetVertexCount(mesh);
        Mesh_AddVertex(mesh, p.x, p.y, p.z, n.x, n.y, n.z, 1, 0);

        /* Generate faces. */
        for (int i = 0; i < 3; ++i) {
          int j = (i + 1) % 3;
          int k = (i + 2) % 3;
          if ((&cell.x)[j] == 0 || (&cell.x)[k] == 0)
            continue;

          int du = (&cellStride.x)[j];
          int dv = (&cellStride.x)[k];

          int i0 = indices[cellIndex];
          int i1 = indices[cellIndex - du];
          int i2 = indices[cellIndex - du - dv];
          int i3 = indices[cellIndex - dv];

          if (i1 < 0 || i2 < 0 || i3 < 0)
            continue;

          if (v[0]->value > 0)
            Mesh_AddQuad(mesh, i0, i3, i2, i1);
          else
            Mesh_AddQuad(mesh, i0, i1, i2, i3);
        }
      }
    }
  }

  MemFree(indices);
  return mesh;
}

void SDF_Clear (SDF* self, float value) {
  uint64 size = self->size.x * self->size.y * self->size.z;
  Cell* pCell = self->data;
  for (uint64 i = 0; i < size; ++i)
    (*pCell++).value = value;
}

void SDF_ComputeNormals (SDF* self) {
  Vec3i const stride = { 1, self->size.x, self->size.x * self->size.y };
  for (int z = 1; z < self->size.z - 1; ++z)
  for (int y = 1; y < self->size.y - 1; ++y)
  for (int x = 1; x < self->size.x - 1; ++x) {
    Cell* cell = self->data + x * stride.x + y * stride.y + z * stride.z;
    Cell const* x0 = cell - stride.x; Cell const* x1 = cell + stride.x;
    Cell const* y0 = cell - stride.y; Cell const* y1 = cell + stride.y;
    Cell const* z0 = cell - stride.z; Cell const* z1 = cell + stride.z;
    cell->normal = Vec3f_SNormalize(Vec3f_Create(
      x1->value - x0->value,
      y1->value - y0->value,
      z1->value - z0->value));
  }
}

void SDF_Set (SDF* self, int x, int y, int z, float value) {
  self->data[x + self->size.x * (y + self->size.y * z)].value = value;
}

void SDF_SetNormal (SDF* self, int x, int y, int z, Vec3f const* normal) {
  self->data[x + self->size.x * (y + self->size.y * z)].normal = *normal;
}

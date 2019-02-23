#include "LodMesh.h"
#include "PhxMemory.h"
#include "Mesh.h"
#include "RefCounted.h"

/* TODO : Merge meshes into single IBO/VBO so that we can skip all the rebinds
 *        (profiling shows that they are a huge perf drain in the rendering
 *         pipeline) */

struct LodMeshEntry {
  LodMeshEntry* next;
  Mesh* mesh;
  float dMin;
  float dMax;
};

struct LodMesh {
  RefCounted;
  LodMeshEntry* head;
};

LodMesh* LodMesh_Create () {
  LodMesh* self = MemNew(LodMesh);
  RefCounted_Init(self);
  self->head = 0;
  return self;
}

void LodMesh_Acquire (LodMesh* self) {
  RefCounted_Acquire(self);
}

void LodMesh_Free (LodMesh* self) {
  RefCounted_Free(self) {
    LodMeshEntry* e = self->head;
    while (e) {
      LodMeshEntry* next = e->next;
      Mesh_Free(e->mesh);
      MemFree(e);
      e = next;
    }
    MemFree(self);
  }
}

void LodMesh_Add (LodMesh* self, Mesh* mesh, float dMin, float dMax) {
  LodMeshEntry* e = MemNew(LodMeshEntry);
  e->mesh = mesh;
  e->dMin = dMin * dMin;
  e->dMax = dMax * dMax;
  e->next = self->head;
  self->head = e;
}

void LodMesh_Draw (LodMesh* self, float d2) {
  for (LodMeshEntry* e = self->head; e; e = e->next)
    if (e->dMin <= d2 && d2 <= e->dMax)
      Mesh_Draw(e->mesh);
}

Mesh* LodMesh_Get (LodMesh* self, float d2) {
  for (LodMeshEntry* e = self->head; e; e = e->next)
    if (e->dMin <= d2 && d2 <= e->dMax)
      return e->mesh;
  return 0;
}

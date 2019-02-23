#include "Draw.h"
#include "Matrix.h"
#include "PhxMemory.h"
#include "Mesh.h"
#include "Octree.h"
#include "Vertex.h"

struct Node {
  Node* next;
  uint64 id;
  Box3f box;
};

struct Octree {
  Box3f box;
  Octree* child[8];
  Node* elems;
};

Octree* Octree_Create (Box3f box) {
  Octree* self = MemNew(Octree);
  MemZero(self, sizeof(Octree));
  self->box = box;
  return self;
}

void Octree_Free (Octree* self) {
  for (int i = 0; i < 8; ++i)
    if (self->child[i])
      Octree_Free(self->child[i]);

  Node* elem = self->elems;
  while (elem) {
    Node* next = elem->next;
    MemFree(elem);
    elem = next;
  }
  MemFree(self);
}

Octree* Octree_FromMesh (Mesh* mesh) {
  Box3f meshBox; Mesh_GetBound(mesh, &meshBox);
  Octree* self = Octree_Create(meshBox);

  int indexCount = Mesh_GetIndexCount(mesh);
  int const* indexData = Mesh_GetIndexData(mesh);
  Vertex const* vertexData = Mesh_GetVertexData(mesh);

  for (int i = 0; i < indexCount; i += 3) {
    Vertex const* v0 = vertexData + indexData[i + 0];
    Vertex const* v1 = vertexData + indexData[i + 1];
    Vertex const* v2 = vertexData + indexData[i + 2];
    Box3f box = Box3f_Create(
      Vec3f_Min(v0->p, Vec3f_Min(v1->p, v2->p)),
      Vec3f_Max(v0->p, Vec3f_Max(v1->p, v2->p)));
    Octree_Add(self, box, i / 3);
  }

  return self;
}

static void Octree_GetAvgLoadImpl (Octree* self, double* load, double* nodes) {
  *nodes += 1;
  for (Node* elem = self->elems; elem; elem = elem->next)
    *load += 1;
  for (int i = 0; i < 8; ++i)
    if (self->child[i])
      Octree_GetAvgLoadImpl(self->child[i], load, nodes);
}

double Octree_GetAvgLoad (Octree* self) {
  double load = 0;
  double nodes = 0;
  Octree_GetAvgLoadImpl(self, &load, &nodes);
  return load / nodes;
}

int Octree_GetMaxLoad (Octree* self) {
  int load = 0;
  for (Node* elem = self->elems; elem; elem = elem->next)
    load += 1;
  for (int i = 0; i < 8; ++i)
    if (self->child[i])
      load = Max(load, Octree_GetMaxLoad(self->child[i]));
  return load;
}

int Octree_GetMemory (Octree* self) {
  int memory = sizeof(Octree);
  for (int i = 0; i < 8; ++i)
    if (self->child[i])
      memory += Octree_GetMemory(self->child[i]);
  for (Node* elem = self->elems; elem; elem = elem->next)
    memory += sizeof(Node);
  return memory;
}

static bool Octree_IntersectRayImpl (Octree* self, Vec3f o, Vec3f di) {
  if (!Box3f_IntersectsRay(self->box, o, di))
    return false;

  for (Node* elem = self->elems; elem; elem = elem->next)
    if (Box3f_IntersectsRay(elem->box, o, di))
      return true;

  for (int i = 0; i < 8; ++i)
    if (self->child[i])
      if (Octree_IntersectRayImpl(self->child[i], o, di))
        return true;

  return false;
}

bool Octree_IntersectRay (
  Octree* self,
  Matrix* matrix,
  Vec3f const* ro,
  Vec3f const* rd)
{
  Matrix* inv = Matrix_Inverse(matrix);
  Vec3f invRo; Matrix_MulPoint(inv, &invRo, ro->x, ro->y, ro->z);
  Vec3f invRd; Matrix_MulDir(inv, &invRd, rd->x, rd->y, rd->z);
  Matrix_Free(inv);
  return Octree_IntersectRayImpl(self, invRo, Vec3f_Rcp(invRd));
}

static void Octree_Insert (Octree* self, Box3f box, uint32 id) {
  Node* elem = MemNew(Node);
  elem->box = box;
  elem->id = id;
  elem->next = self->elems;
  self->elems = elem;
}

static void Octree_AddDepth (Octree* self, Box3f box, uint32 id, int depth) {
  Vec3f const* const NO_ALIAS L = &self->box.lower;
  Vec3f const* const NO_ALIAS U = &self->box.upper;
  Vec3f const C = Box3f_Center(self->box);
  Box3f const childBound[8] = {
    { { L->x, L->y, L->z }, {  C.x,  C.y,  C.z } },
    { {  C.x, L->y, L->z }, { U->x,  C.y,  C.z } },
    { { L->x,  C.y, L->z }, {  C.x, U->y,  C.z } },
    { {  C.x,  C.y, L->z }, { U->x, U->y,  C.z } },
    { { L->x, L->y,  C.z }, {  C.x,  C.y, U->z } },
    { {  C.x, L->y,  C.z }, { U->x,  C.y, U->z } },
    { { L->x,  C.y,  C.z }, {  C.x, U->y, U->z } },
    { {  C.x,  C.y,  C.z }, { U->x, U->y, U->z } }
  };

  int intersections = 0;
  int lastIntersection = 0;
  for (int i = 0; i < 8; ++i) {
    if (Box3f_IntersectsBox(box, childBound[i])) {
      intersections++;
      lastIntersection = i;
    }
  }

  if (intersections == 0)
    return;

  if (intersections == 1) {
    if (!self->child[lastIntersection])
      self->child[lastIntersection] = Octree_Create(childBound[lastIntersection]);
    Octree_AddDepth(
      self->child[lastIntersection],
      Box3f_Intersection(box, childBound[lastIntersection]), id, depth + 1);
    return;
  }

  Octree_Insert(self, box, id);
}

void Octree_Add (Octree* self, Box3f box, uint32 id) {
  Octree_AddDepth(self, box, id, 0);
}

void Octree_Draw (Octree* self) {
  Draw_Color(1, 1, 1, 1);
  Draw_Box3(&self->box);
  Draw_Color(0, 1, 0, 1);
  for (Node* elem = self->elems; elem; elem = elem->next)
    Draw_Box3(&elem->box);
  for (int i = 0; i < 8; ++i)
    if (self->child[i])
      Octree_Draw(self->child[i]);
}

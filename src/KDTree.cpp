#include "Draw.h"
#include "KDTree.h"
#include "PhxMemory.h"
#include "Mesh.h"
#include "Vertex.h"
#include <stdlib.h>

int const kMaxLeafSize = 64;

struct Node {
  Node* next;
  uint64 id;
  Box3f box;
};

struct KDTree {
  Box3f box;
  KDTree* back;
  KDTree* front;
  Node* elems;
};

static int compareLowerX (void const* a, void const* b) {
  return (((Box3f const*)a)->lower.x < ((Box3f const*)b)->lower.x) ? -1 : 1;
}

static int compareLowerY (void const* a, void const* b) {
  return (((Box3f const*)a)->lower.y < ((Box3f const*)b)->lower.y) ? -1 : 1;
}

static int compareLowerZ (void const* a, void const* b) {
  return (((Box3f const*)a)->lower.z < ((Box3f const*)b)->lower.z) ? -1 : 1;
}

static KDTree* Partition (Box3f* boxes, int boxCount, int dim) {
  KDTree* self = MemNew(KDTree);
  if (boxCount <= kMaxLeafSize) {
    self->box = boxes[0];
    self->back = 0;
    self->front = 0;
    self->elems = 0;
    for (int i = 1; i < boxCount; ++i)
      self->box = Box3f_Union(self->box, boxes[i]);

    for (int i = 0; i < boxCount; ++i) {
      Node* node = MemNew(Node);
      node->box = boxes[i];
      node->next = self->elems;
      node->id = 0;
      self->elems = node;
    }
    return self;
  }

  if (dim == 0) qsort(boxes, boxCount, sizeof(Box3f), compareLowerX);
  if (dim == 1) qsort(boxes, boxCount, sizeof(Box3f), compareLowerY);
  if (dim == 2) qsort(boxes, boxCount, sizeof(Box3f), compareLowerZ);

  int boxCountBack = boxCount / 2;
  int boxCountFront = boxCount - boxCountBack;
  Box3f* boxesBack = MemNewArray(Box3f, boxCountBack);
  Box3f* boxesFront = MemNewArray(Box3f, boxCountFront);
  MemCpy(boxesBack, boxes, boxCountBack * sizeof(Box3f));
  MemCpy(boxesFront, boxes + boxCountBack, boxCountFront * sizeof(Box3f));

  self->back = Partition(boxesBack, boxCountBack, (dim + 1) % 3);
  self->front = Partition(boxesFront, boxCountFront, (dim + 1) % 3);
  self->box = Box3f_Union(self->back->box, self->front->box);
  self->elems = 0;

  MemFree(boxesBack);
  MemFree(boxesFront);
  return self;
}

KDTree* KDTree_FromMesh (Mesh* mesh) {
  int const indexCount = Mesh_GetIndexCount(mesh);
  int const* indexData = Mesh_GetIndexData(mesh);
  Vertex const* vertexData = Mesh_GetVertexData(mesh);

  int const boxCount = indexCount / 3;
  Box3f* boxes = MemNewArray(Box3f, boxCount);

  for (int i = 0; i < indexCount; i += 3) {
    Vertex const* v0 = vertexData + indexData[i + 0];
    Vertex const* v1 = vertexData + indexData[i + 1];
    Vertex const* v2 = vertexData + indexData[i + 2];
    boxes[i / 3] = Box3f_Create(
      Vec3f_Min(v0->p, Vec3f_Min(v1->p, v2->p)),
      Vec3f_Max(v0->p, Vec3f_Max(v1->p, v2->p)));
  }

  KDTree* self = Partition(boxes, boxCount, 0);
  MemFree(boxes);
  return self;
}

void KDTree_Free (KDTree* self) {
  if (self->back) KDTree_Free(self->back);
  if (self->front) KDTree_Free(self->front);
  Node* elem = self->elems;
  while (elem) {
    Node* next = elem->next;
    MemFree(elem);
    elem = next;
  }
  MemFree(self);
}

int KDTree_GetMemory (KDTree* self) {
  int memory = sizeof(KDTree);
  if (self->back) memory += KDTree_GetMemory(self->back);
  if (self->front) memory += KDTree_GetMemory(self->front);
  for (Node* elem = self->elems; elem; elem = elem->next)
    memory += sizeof(Node);
  return memory;
}

bool KDTree_IntersectRay (KDTree*, Matrix*, Vec3f const*, Vec3f const*) {
  return false;
}

void KDTree_Draw (KDTree* self, int maxDepth) {
  if (maxDepth < 0) return;
  Draw_Color(1, 1, 1, 1);
  Draw_Box3(&self->box);
  if (self->back) KDTree_Draw(self->back, maxDepth - 1);
  if (self->front) KDTree_Draw(self->front, maxDepth - 1);
#if 0
  Draw_Color(0, 1, 0, 1);
  for (Node* elem = self->elems; elem; elem = elem->next)
    Draw_Box3(&elem->box);
#endif
}

#include "BoxTree.h"
#include "Draw.h"
#include "Matrix.h"
#include "PhxMemory.h"
#include "Mesh.h"
#include "RNG.h"
#include "Vertex.h"

struct Node {
  Box3f box;
  void* data;
  Node* sub[2];
};

struct BoxTree {
  Node* root;
};

inline static Node* Node_Create (Box3f box, void* data) {
  Node* self = MemNew(Node);
  self->box = box;
  self->sub[0] = 0;
  self->sub[1] = 0;
  self->data = data;
  return self;
}

BoxTree* BoxTree_Create () {
  BoxTree* self = MemNew(BoxTree);
  self->root = 0;
  return self;
}

static void Node_Free (Node* self) {
  if (self->sub[0]) Node_Free(self->sub[0]);
  if (self->sub[1]) Node_Free(self->sub[1]);
  MemFree(self);
}

void BoxTree_Free (BoxTree* self) {
  if (self->root) Node_Free(self->root);
  MemFree(self);
}

BoxTree* BoxTree_FromMesh (Mesh* mesh) {
  BoxTree* self = BoxTree_Create();
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
    BoxTree_Add(self, box, 0);
  }
  return self;
}

inline static float Cost (Box3f box) {
  return Box3f_Volume(box);
}

inline static float CostMerge (Box3f a, Box3f b) {
  return Cost(Box3f_Union(a, b));
}

static void Node_Merge (Node* self, Node* src, Node** prev) {
  if (!self) {
    *prev = src;
    return;
  }

  /* Leaf node. */
  if (!self->sub[0]) {
    Node* parent = Node_Create(Box3f_Union(self->box, src->box), 0);
    *prev = parent;
    parent->sub[0] = self;
    parent->sub[1] = src;
    self = parent;
    return;
  }

  /* Contained by current sub-tree. */
  if (Box3f_ContainsBox(self->box, src->box)) {
    float cost0 = CostMerge(self->sub[0]->box, src->box) + Cost(self->sub[1]->box);
    float cost1 = CostMerge(self->sub[1]->box, src->box) + Cost(self->sub[0]->box);
    if (cost0 < cost1)
      Node_Merge(self->sub[0], src, &self->sub[0]);
    else
      Node_Merge(self->sub[1], src, &self->sub[1]);
  }

  /* Not contained, need new parent. */
  else {
    Node* parent = Node_Create(Box3f_Union(self->box, src->box), 0);
    *prev = parent;

    float costBase = Cost(self->box) + Cost(src->box);
    float cost0 = CostMerge(self->sub[0]->box, src->box) + Cost(self->sub[1]->box);
    float cost1 = CostMerge(self->sub[1]->box, src->box) + Cost(self->sub[0]->box);

    if (costBase <= cost0 && costBase <= cost1) {
      parent->sub[0] = self;
      parent->sub[1] = src;
    }

    else if (cost0 <= costBase && cost0 <= cost1) {
      parent->sub[0] = self->sub[0];
      parent->sub[1] = self->sub[1];
      MemFree(self);
      Node_Merge(parent->sub[0], src, &parent->sub[0]);
    }

    else {
      parent->sub[0] = self->sub[0];
      parent->sub[1] = self->sub[1];
      MemFree(self);
      Node_Merge(parent->sub[1], src, &parent->sub[1]);
    }
  }
}

void BoxTree_Add (BoxTree* self, Box3f box, void* data) {
  Node_Merge(self->root, Node_Create(box, data), &self->root);
}

static int Node_GetMemory (Node* self) {
  int memory = sizeof(Node);
  if (self->sub[0]) memory += Node_GetMemory(self->sub[0]);
  if (self->sub[1]) memory += Node_GetMemory(self->sub[1]);
  return memory;
}

int BoxTree_GetMemory (BoxTree* self) {
  int memory = sizeof(BoxTree);
  if (self->root) memory += Node_GetMemory(self->root);
  return memory;
}

static bool Node_IntersectRay (Node* self, Vec3f o, Vec3f di) {
  if (!Box3f_IntersectsRay(self->box, o, di))
    return false;

  if (self->sub[0]) {
    if (Node_IntersectRay(self->sub[0], o, di)) return true;
    if (Node_IntersectRay(self->sub[1], o, di)) return true;
    return false;
  } else {
    return true;
  }
}

bool BoxTree_IntersectRay (
  BoxTree* self,
  Matrix* matrix,
  Vec3f const* ro,
  Vec3f const* rd)
{
  if (!self->root) return false;
  Matrix* inv = Matrix_Inverse(matrix);
  Vec3f invRo; Matrix_MulPoint(inv, &invRo, ro->x, ro->y, ro->z);
  Vec3f invRd; Matrix_MulDir(inv, &invRd, rd->x, rd->y, rd->z);
  Matrix_Free(inv);
  return Node_IntersectRay(self->root, invRo, Vec3f_Rcp(invRd));
}

static void BoxTree_DrawNode (Node* self, int maxDepth) {
  if (maxDepth < 0) return;
  if (self->sub[0] || self->sub[1]) {
    Draw_Color(1, 1, 1, 1);
    Draw_Box3(&self->box);
  } else {
    Draw_Color(0, 1, 0, 1);
    Draw_Box3(&self->box);
  }
  if (self->sub[0]) BoxTree_DrawNode(self->sub[0], maxDepth - 1);
  if (self->sub[1]) BoxTree_DrawNode(self->sub[1], maxDepth - 1);
}

void BoxTree_Draw (BoxTree* self, int maxDepth) {
  if (self->root)
    BoxTree_DrawNode(self->root, maxDepth);
}

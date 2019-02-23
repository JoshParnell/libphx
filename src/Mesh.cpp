#include "ArrayList.h"
#include "Box3.h"
#include "Bytes.h"
#include "Matrix.h"
#include "Mesh.h"
#include "Metric.h"
#include "OpenGL.h"
#include "RefCounted.h"
#include "Resource.h"
#include "SDF.h"
#include "Triangle.h"
#include "Vertex.h"

#include <float.h>

struct Computed {
  Box3f bound;
  float radius;
};

struct Mesh {
  RefCounted;
  uint vbo;
  uint ibo;
  uint64 version;
  uint64 versionBuffers;
  uint64 versionInfo;
  Computed info;
  ArrayList(int32, index);
  ArrayList(Vertex, vertex);
};

static void Mesh_UpdateInfo (Mesh* self) {
  if (self->versionInfo == self->version)
    return;
  self->info.bound.lower = Vec3f_Create( FLT_MAX,  FLT_MAX,  FLT_MAX);
  self->info.bound.upper = Vec3f_Create(-FLT_MAX, -FLT_MAX, -FLT_MAX);

  /* Compute bounding box. */
  ArrayList_ForEach(self->vertex, Vertex, v) {
    Box3f_Add(&self->info.bound, v->p);
  }

  Vec3f center = Box3f_Center(self->info.bound);

  /* Compute radius in second pass after bound is. Note that radius is relative
   * to bounding box center, NOT centroid! */ {
    double r2 = 0.0;
    ArrayList_ForEach(self->vertex, Vertex, v) {
      double dx = v->p.x - center.x;
      double dy = v->p.y - center.y;
      double dz = v->p.z - center.z;
      r2 = Max(r2, dx*dx + dy*dy + dz*dz);
    }
    self->info.radius = (float)Sqrt(r2);
  }
  self->versionInfo = self->version;
}

Mesh* Mesh_Create () {
  Mesh* self = MemNew(Mesh);
  RefCounted_Init(self);
  self->vbo = 0;
  self->ibo = 0;
  self->version = 1;
  self->versionBuffers = 0;
  self->versionInfo = 0;
  ArrayList_Init(self->vertex);
  ArrayList_Init(self->index);
  return self;
}

Mesh* Mesh_Clone (Mesh* other) {
  Mesh* self = Mesh_Create();
  ArrayList_Reserve(self->index, other->index_size);
  ArrayList_Reserve(self->vertex, other->vertex_size);
  self->index_size = other->index_size;
  self->vertex_size = other->vertex_size;
  MemCpy(self->index_data, other->index_data, sizeof(int) * other->index_size);
  MemCpy(self->vertex_data, other->vertex_data, sizeof(Vertex) * other->vertex_size);
  return self;
}

Mesh* Mesh_Load (cstr name) {
  Bytes* bytes = Resource_LoadBytes(ResourceType_Mesh, name);
  Mesh* self = Mesh_FromBytes(bytes);
  Bytes_Free(bytes);
  return self;
}

void Mesh_Acquire (Mesh* self) {
  RefCounted_Acquire(self);
}

void Mesh_Free (Mesh* self) {
  RefCounted_Free(self) {
    ArrayList_Free(self->vertex);
    ArrayList_Free(self->index);
    if (self->vbo) {
      GLCALL(glDeleteBuffers(1, &self->vbo))
      GLCALL(glDeleteBuffers(1, &self->ibo))
    }
    MemFree(self);
  }
}

Bytes* Mesh_ToBytes (Mesh* mesh) {
  int32 vertexCount = Mesh_GetVertexCount(mesh);
  int32 indexCount = Mesh_GetIndexCount(mesh);
  uint32 size = 2 * sizeof(int32) +
    vertexCount * sizeof(Vertex) +
    indexCount * sizeof(int32);

  Bytes* self = Bytes_Create(size);
  Bytes_WriteI32(self, vertexCount);
  Bytes_WriteI32(self, indexCount);
  Bytes_Write(self, ArrayList_GetData(mesh->vertex), vertexCount * sizeof(Vertex));
  Bytes_Write(self, ArrayList_GetData(mesh->index), indexCount * sizeof(int32));
  return self;
}

Mesh* Mesh_FromBytes (Bytes* buf) {
  Mesh* self = Mesh_Create();
  int32 vertexCount = Bytes_ReadI32(buf);
  int32 indexCount = Bytes_ReadI32(buf);
  Mesh_ReserveVertexData(self, vertexCount);
  Mesh_ReserveIndexData(self, indexCount);
  Bytes_Read(buf, ArrayList_GetData(self->vertex), vertexCount * sizeof(Vertex));
  Bytes_Read(buf, ArrayList_GetData(self->index), indexCount * sizeof(int32));
  self->vertex_size = vertexCount;
  self->index_size = indexCount;
  return self;
}

Mesh* Mesh_FromSDF (SDF* sdf) {
  return SDF_ToMesh(sdf);
}

void Mesh_AddIndex (Mesh* self, int newIndex) {
  ArrayList_Grow(self->index);
  self->index_data[self->index_size++] = newIndex;
  self->version++;
}

void Mesh_AddMesh (Mesh* self, Mesh* other) {
  int indexOffset = self->vertex_size;
  for (int i = 0; i < other->vertex_size; ++i)
    Mesh_AddVertexRaw(self, other->vertex_data + i);
  for (int i = 0; i < other->index_size; ++i)
    Mesh_AddIndex(self, other->index_data[i] + indexOffset);
}

void Mesh_AddQuad (Mesh* self, int i1, int i2, int i3, int i4) {
  Mesh_AddTri(self, i1, i2, i3);
  Mesh_AddTri(self, i1, i3, i4);
}

void Mesh_AddTri (Mesh* self, int i1, int i2, int i3) {
  Mesh_AddIndex(self, i1);
  Mesh_AddIndex(self, i2);
  Mesh_AddIndex(self, i3);
}

void Mesh_AddVertex (Mesh* self,
  float px, float py, float pz,
  float nx, float ny, float nz,
  float u, float v)
{
  ArrayList_Grow(self->vertex);
  Vertex* newVertex = self->vertex_data + (self->vertex_size++);
  newVertex->p = Vec3f_Create(px, py, pz);
  newVertex->n = Vec3f_Create(nx, ny, nz);
  newVertex->uv = Vec2f_Create(u, v);
  self->version++;
}

void Mesh_AddVertexRaw (Mesh* self, Vertex const* vertex) {
  ArrayList_Grow(self->vertex);
  *(self->vertex_data + (self->vertex_size++)) = *vertex;
  self->version++;
}

void Mesh_DrawBind (Mesh* self) {
  /* Release cached GL buffers if the mesh has changed since we built them. */
  if (self->vbo && self->version != self->versionBuffers) {
    GLCALL(glDeleteBuffers(1, &self->vbo))
    GLCALL(glDeleteBuffers(1, &self->ibo))
    self->vbo = 0;
    self->ibo = 0;
  }

  /* Generate cached GL buffers for fast drawing. */
  if (!self->vbo) {
    GLCALL(glGenBuffers(1, &self->vbo))
    GLCALL(glGenBuffers(1, &self->ibo))
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, self->vbo))
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ibo))
    GLCALL(glBufferData(
      GL_ARRAY_BUFFER,
      self->vertex_size * sizeof(Vertex),
      self->vertex_data,
      GL_STATIC_DRAW))

    /* TODO : 16-bit index optimization */
    /* TODO : Check if 8-bit indices are supported by hardware. IIRC they
     *        weren't last time I checked. */

    GLCALL(glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      self->index_size * sizeof(int),
      self->index_data,
      GL_STATIC_DRAW))

    self->versionBuffers = self->version;
  }

  GLCALL(glBindBuffer(GL_ARRAY_BUFFER, self->vbo))
  GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ibo))

  GLCALL(glEnableVertexAttribArray(0))
  GLCALL(glEnableVertexAttribArray(1))
  GLCALL(glEnableVertexAttribArray(2))

  GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void const*)OFFSET_OF(Vertex, p)))
  GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void const*)OFFSET_OF(Vertex, n)))
  GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void const*)OFFSET_OF(Vertex, uv)))
}

void Mesh_DrawBound (Mesh* self) {
  Metric_AddDraw(self->index_size / 3, self->index_size / 3, self->vertex_size);
  GLCALL(glDrawElements(GL_TRIANGLES, self->index_size, GL_UNSIGNED_INT, 0))
}

void Mesh_DrawUnbind (Mesh*) {
  GLCALL(glDisableVertexAttribArray(0))
  GLCALL(glDisableVertexAttribArray(1))
  GLCALL(glDisableVertexAttribArray(2))
  GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0))
  GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0))
}

void Mesh_Draw (Mesh* self) {
  Mesh_DrawBind(self);
  Mesh_DrawBound(self);
  Mesh_DrawUnbind(self);
}

void Mesh_DrawNormals (Mesh* self, float scale) {
  glBegin(GL_LINES);
  ArrayList_ForEach(self->vertex, Vertex, v) {
    glVertex3f(v->p.x, v->p.y, v->p.z);
    glVertex3f(
      v->p.x + scale * v->n.x,
      v->p.y + scale * v->n.y,
      v->p.z + scale * v->n.z);
  }
  GLCALL(glEnd())
}

void Mesh_GetBound (Mesh* self, Box3f* out) {
  Mesh_UpdateInfo(self);
  *out = self->info.bound;
}

void Mesh_GetCenter (Mesh* self, Vec3f* out) {
  Mesh_UpdateInfo(self);
  *out = Box3f_Center(self->info.bound);
}

int Mesh_GetIndexCount (Mesh* self) {
  return self->index_size;
}

int* Mesh_GetIndexData (Mesh* self) {
  return self->index_data;
}

float Mesh_GetRadius (Mesh* self) {
  Mesh_UpdateInfo(self);
  return self->info.radius;
}

uint64 Mesh_GetVersion (Mesh* self) {
  return self->version;
}

void Mesh_IncVersion (Mesh* self) {
  self->version++;
}

Error Mesh_Validate (Mesh* self) {
  int32   indexLen   = Mesh_GetIndexCount(self);
  int32*  indexData  = Mesh_GetIndexData(self);
  Vertex* vertexData = Mesh_GetVertexData(self);

  if (indexLen % 3 != 0) return Error_Index | Error_BadCount;

  for (int32 i = 0; i < indexLen; i += 3) {
    int32 i0 = indexData[i + 0];
    int32 i1 = indexData[i + 1];
    int32 i2 = indexData[i + 2];

    Triangle triangle;
    triangle.vertices[0] = vertexData[i0].p;
    triangle.vertices[1] = vertexData[i1].p;
    triangle.vertices[2] = vertexData[i2].p;

    Error e = Triangle_Validate(&triangle);
    if (e != Error_None) return Error_VertPos | e;
  }

  ArrayList_ForEach(self->vertex, Vertex const, v) {
    Error e;
    if ((e = Vec3f_Validate(v->p))  != Error_None) return Error_VertPos  | e;
    if ((e = Vec3f_Validate(v->n))  != Error_None) return Error_VertNorm | e;
    if ((e = Vec2f_Validate(v->uv)) != Error_None) return Error_VertUV   | e;
  }

  return Error_None;
}

Vertex* Mesh_GetVertex (Mesh* self, int index) {
  return self->vertex_data + index;
}

int Mesh_GetVertexCount (Mesh* self) {
  return self->vertex_size;
}

Vertex* Mesh_GetVertexData (Mesh* self) {
  return self->vertex_data;
}

void Mesh_ReserveIndexData (Mesh* self, int capacity) {
  ArrayList_Reserve(self->index, capacity);
}

void Mesh_ReserveVertexData (Mesh* self, int capacity) {
  ArrayList_Reserve(self->vertex, capacity);
}

Mesh* Mesh_Center (Mesh* self) {
  Vec3f c; Mesh_GetCenter(self, &c);
  Mesh_Translate(self, -c.x, -c.y, -c.z);
  return self;
}

Mesh* Mesh_Invert (Mesh* self) {
  for (int i = 0; i < self->index_size; i += 3)
    Swap(self->index_data[i + 1], self->index_data[i + 2]);
  self->version++;
  return self;
}

Mesh* Mesh_RotateX (Mesh* self, float rads) {
  Matrix* matrix = Matrix_RotationX(rads);
  Mesh_Transform(self, matrix);
  Matrix_Free(matrix);
  return self;
}

Mesh* Mesh_RotateY (Mesh* self, float rads) {
  Matrix* matrix = Matrix_RotationY(rads);
  Mesh_Transform(self, matrix);
  Matrix_Free(matrix);
  return self;
}

Mesh* Mesh_RotateZ (Mesh* self, float rads) {
  Matrix* matrix = Matrix_RotationZ(rads);
  Mesh_Transform(self, matrix);
  Matrix_Free(matrix);
  return self;
}

Mesh* Mesh_RotateYPR (Mesh* self, float yaw, float pitch, float roll) {
  Matrix* matrix = Matrix_YawPitchRoll(yaw, pitch, roll);
  Mesh_Transform(self, matrix);
  Matrix_Free(matrix);
  return self;
}

Mesh* Mesh_Scale (Mesh* self, float x, float y, float z) {
  ArrayList_ForEach(self->vertex, Vertex, v) {
    v->p.x *= x;
    v->p.y *= y;
    v->p.z *= z;
  }
  self->version++;
  return self;
}

Mesh* Mesh_ScaleUniform (Mesh* self, float s) {
  Mesh_Scale(self, s, s, s);
  return self;
}

Mesh* Mesh_Translate (Mesh* self, float x, float y, float z) {
  ArrayList_ForEach(self->vertex, Vertex, v) {
    v->p.x += x;
    v->p.y += y;
    v->p.z += z;
  }
  self->version++;
  return self;
}

Mesh* Mesh_Transform (Mesh* self, Matrix* NO_ALIAS matrix) {
  ArrayList_ForEach(self->vertex, Vertex, v)
    Matrix_MulPoint(matrix, &v->p, v->p.x, v->p.y, v->p.z);
  self->version++;
  return self;
}

void Mesh_ComputeNormals (Mesh* self) {
  ArrayList_ForEach(self->vertex, Vertex, v) {
    v->n.x = 0;
    v->n.y = 0;
    v->n.z = 0;
  }

  for (int i = 0; i < self->index_size; i += 3) {
    Vertex* v1 = ArrayList_GetPtr(self->vertex, ArrayList_Get(self->index, i + 0));
    Vertex* v2 = ArrayList_GetPtr(self->vertex, ArrayList_Get(self->index, i + 1));
    Vertex* v3 = ArrayList_GetPtr(self->vertex, ArrayList_Get(self->index, i + 2));

    Vec3f e1 = Vec3f_Sub(v2->p, v1->p);
    Vec3f e2 = Vec3f_Sub(v3->p, v2->p);
    Vec3f en = Vec3f_Cross(e1, e2);
    v1->n = Vec3f_Add(v1->n, en);
    v2->n = Vec3f_Add(v2->n, en);
    v3->n = Vec3f_Add(v3->n, en);
  }

  ArrayList_ForEach(self->vertex, Vertex, v)
    v->n = Vec3f_Normalize(v->n);

  self->version++;
}

void Mesh_SplitNormals (Mesh* self, float minDot) {
  ArrayList_ForEach(self->vertex, Vertex, v)
    v->n = Vec3f_Create(0, 0, 0);

  for (int i = 0; i < self->index_size; i += 3) {
    int32* index[3] = {
      self->index_data + i + 0,
      self->index_data + i + 1,
      self->index_data + i + 2,
    };

    Vertex* v[3] = {
      self->vertex_data + *index[0],
      self->vertex_data + *index[1],
      self->vertex_data + *index[2],
    };

    Vec3f face = Vec3f_Cross(
      Vec3f_Sub(v[1]->p, v[0]->p),
      Vec3f_Sub(v[2]->p, v[0]->p));

    for (int j = 0; j < 3; ++j) {
      Vec3f* cn = &self->vertex_data[*index[j]].n;
      if (Vec3f_LengthSquared(*cn) > 0.0f) {
        float cDot = Vec3f_Dot(Vec3f_Normalize(face), Vec3f_Normalize(*cn));
        if (cDot < minDot) {
          ArrayList_Grow(self->vertex);
          Vertex* nv = self->vertex_data + (self->vertex_size++);
          *nv = ArrayList_Get(self->vertex, *index[j]);
          nv->n = face;
          *index[j] = ArrayList_GetSize(self->vertex) - 1;
        } else {
          Vec3f_IAdd(cn, face);
        }
      } else {
        Vec3f_IAdd(cn, face);
      }
    }
  }

  ArrayList_ForEach(self->vertex, Vertex, v)
    v->n = Vec3f_Normalize(v->n);
}

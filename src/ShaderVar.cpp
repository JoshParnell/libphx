#include "Matrix.h"
#include "PhxMemory.h"
#include "ShaderVar.h"
#include "ShaderVarType.h"
#include "StrMap.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

#define DEFAULT_CAPACITY 4

struct VarStack {
  ShaderVarType type;
  int32 size;
  int32 capacity;
  int32 elemSize;
  void* data;
};

static StrMap* varMap = 0;

inline static VarStack* ShaderVar_GetStack (cstr var, ShaderVarType type) {
  VarStack* self = (VarStack*)StrMap_Get(varMap, var);
  if (!self) {
    if (!type)
      return 0;
    self = MemNew(VarStack);
    self->type = type;
    self->size = 0;
    self->capacity = DEFAULT_CAPACITY;
    self->elemSize = ShaderVarType_GetSize(type);
    self->data = MemAlloc(self->capacity * self->elemSize);
    StrMap_Set(varMap, var, self);
  }

  if (type && self->type != type)
    Fatal("ShaderVar_GetStack: Attempting to get stack of type <%s>"
          " for shader variable <%s> when existing stack has type <%s>",
          ShaderVarType_GetName(type), var, ShaderVarType_GetName(self->type));

  return self;
}

inline static void ShaderVar_Push (cstr var, ShaderVarType type, void const* value) {
  VarStack* self = ShaderVar_GetStack(var, type);
  if (self->size == self->capacity) {
    self->capacity *= 2;
    self->data = MemRealloc(self->data, self->capacity * self->elemSize);
  }
  MemCpy((char*)self->data + self->size * self->elemSize, value, self->elemSize);
  self->size++;
}

void ShaderVar_Init () {
  varMap = StrMap_Create(16);
}

void ShaderVar_Free () {
  StrMap_Free(varMap);
  varMap = 0;
}

void* ShaderVar_Get (cstr name, ShaderVarType type) {
  VarStack* self = ShaderVar_GetStack(name, 0);
  if (!self || self->size == 0)
    return 0;
  if (type && self->type != type)
    Fatal("ShaderVar_Get: Attempting to get variable <%s> with type <%s> when"
          " existing stack has type <%s>",
          name, ShaderVarType_GetName(type), ShaderVarType_GetName(self->type));
  return (char*)self->data + self->elemSize * (self->size - 1);
}

void ShaderVar_PushFloat (cstr name, float x) {
  ShaderVar_Push(name, ShaderVarType_Float, &x);
}

void ShaderVar_PushFloat2 (cstr name, float x, float y) {
  Vec2f value = { x, y };
  ShaderVar_Push(name, ShaderVarType_Float2, &value);
}

void ShaderVar_PushFloat3 (cstr name, float x, float y, float z) {
  Vec3f value = { x, y, z };
  ShaderVar_Push(name, ShaderVarType_Float3, &value);
}

void ShaderVar_PushFloat4 (cstr name, float x, float y, float z, float w) {
  Vec4f value = { x, y, z, w };
  ShaderVar_Push(name, ShaderVarType_Float4, &value);
}

void ShaderVar_PushInt (cstr name, int x) {
  int32 value = (int32)x;
  ShaderVar_Push(name, ShaderVarType_Int, &value);
}

void ShaderVar_PushMatrix (cstr name, Matrix* x) {
  ShaderVar_Push(name, ShaderVarType_Matrix, &x);
}

void ShaderVar_PushTex1D (cstr name, Tex1D* x) {
  ShaderVar_Push(name, ShaderVarType_Tex1D, &x);
}

void ShaderVar_PushTex2D (cstr name, Tex2D* x) {
  ShaderVar_Push(name, ShaderVarType_Tex2D, &x);
}

void ShaderVar_PushTex3D (cstr name, Tex3D* x) {
  ShaderVar_Push(name, ShaderVarType_Tex3D, &x);
}

void ShaderVar_PushTexCube (cstr name, TexCube* x) {
  ShaderVar_Push(name, ShaderVarType_TexCube, &x);
}

void ShaderVar_Pop (cstr name) {
  VarStack* self = ShaderVar_GetStack(name, 0);
  if (!self)
    Fatal("ShaderVar_Pop: Attempting to pop nonexistent stack <%s>", name);
  if (self->size == 0)
    Fatal("ShaderVar_Pop: Attempting to pop empty stack <%s>", name);
  self->size--;
}

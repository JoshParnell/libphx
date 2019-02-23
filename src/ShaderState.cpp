#include "ArrayList.h"
#include "Matrix.h"
#include "PhxMemory.h"
#include "OpenGL.h"
#include "RefCounted.h"
#include "Shader.h"
#include "ShaderState.h"
#include "Tex1D.h"
#include "Tex2D.h"
#include "Tex3D.h"
#include "TexCube.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

const uint32 ElemType_Float   =  1;
const uint32 ElemType_Float2  =  2;
const uint32 ElemType_Float3  =  3;
const uint32 ElemType_Float4  =  4;
const uint32 ElemType_Int     =  5;
const uint32 ElemType_Matrix  =  6;
const uint32 ElemType_Tex1D   =  7;
const uint32 ElemType_Tex2D   =  8;
const uint32 ElemType_Tex3D   =  9;
const uint32 ElemType_TexCube = 10;

struct Elem {
  uint32 type;
  int32 index;
  union {
    float    asFloat;
    Vec2f    asFloat2;
    Vec3f    asFloat3;
    Vec4f    asFloat4;
    int      asInt;
    Matrix*  asMatrix;
    Tex1D*   asTex1D;
    Tex2D*   asTex2D;
    Tex3D*   asTex3D;
    TexCube* asTexCube;
  } data;
};

struct ShaderState {
  RefCounted;
  Shader* shader;
  ArrayList(Elem, elems);
};

ShaderState* ShaderState_Create (Shader* shader) {
  ShaderState* self = MemNew(ShaderState);
  RefCounted_Init(self);
  ArrayList_Init(self->elems);
  Shader_Acquire(shader);
  self->shader = shader;
  return self;
}

void ShaderState_Acquire (ShaderState* self) {
  RefCounted_Acquire(self);
}

void ShaderState_Free (ShaderState* self) {
  RefCounted_Free(self) {
    ArrayList_ForEach(self->elems, Elem, e) {
      switch (e->type) {
        case ElemType_Tex1D:
          Tex1D_Free(e->data.asTex1D);
          break;

        case ElemType_Tex2D:
          Tex2D_Free(e->data.asTex2D);
          break;

        case ElemType_Tex3D:
          Tex3D_Free(e->data.asTex3D);
          break;

        case ElemType_TexCube:
          TexCube_Free(e->data.asTexCube);
          break;
      }
    }

    Shader_Free(self->shader);
    ArrayList_Free(self->elems);
    MemFree(self);
  }
}

ShaderState* ShaderState_FromShaderLoad (cstr vertName, cstr fragName) {
  Shader* shader = Shader_Load(vertName, fragName);
  ShaderState* self = ShaderState_Create(shader);
  Shader_Free(shader);
  return self;
}

void ShaderState_SetFloat (ShaderState* self, cstr name, float x) {
  Elem elem = { ElemType_Float, Shader_GetVariable(self->shader, name) };
  elem.data.asFloat = x;
  ArrayList_Append(self->elems, elem);
}

void ShaderState_SetFloat2 (ShaderState* self, cstr name, float x, float y) {
  Elem elem = { ElemType_Float2, Shader_GetVariable(self->shader, name) };
  elem.data.asFloat2 = Vec2f_Create(x, y);
  ArrayList_Append(self->elems, elem);
}

void ShaderState_SetFloat3 (ShaderState* self, cstr name, float x, float y, float z) {
  Elem elem = { ElemType_Float3, Shader_GetVariable(self->shader, name) };
  elem.data.asFloat3 = Vec3f_Create(x, y, z);
  ArrayList_Append(self->elems, elem);
}

void ShaderState_SetFloat4 (ShaderState* self, cstr name, float x, float y, float z, float w) {
  Elem elem = { ElemType_Float4, Shader_GetVariable(self->shader, name) };
  elem.data.asFloat4 = Vec4f_Create(x, y, z, w);
  ArrayList_Append(self->elems, elem);
}

void ShaderState_SetInt (ShaderState* self, cstr name, int x) {
  Elem elem = { ElemType_Int, Shader_GetVariable(self->shader, name) };
  elem.data.asInt = x;
  ArrayList_Append(self->elems, elem);
}

void ShaderState_SetMatrix (ShaderState* self, cstr name, Matrix* x) {
  Elem elem = { ElemType_Matrix, Shader_GetVariable(self->shader, name) };
  elem.data.asMatrix = x;
  ArrayList_Append(self->elems, elem);
}

void ShaderState_SetTex1D (ShaderState* self, cstr name, Tex1D* x) {
  Tex1D_Acquire(x);
  Elem elem = { ElemType_Tex1D, Shader_GetVariable(self->shader, name) };
  elem.data.asTex1D = x;
  ArrayList_Append(self->elems, elem);
}

void ShaderState_SetTex2D (ShaderState* self, cstr name, Tex2D* x) {
  Tex2D_Acquire(x);
  Elem elem = { ElemType_Tex2D, Shader_GetVariable(self->shader, name) };
  elem.data.asTex2D = x;
  ArrayList_Append(self->elems, elem);
}

void ShaderState_SetTex3D (ShaderState* self, cstr name, Tex3D* x) {
  Tex3D_Acquire(x);
  Elem elem = { ElemType_Tex3D, Shader_GetVariable(self->shader, name) };
  elem.data.asTex3D = x;
  ArrayList_Append(self->elems, elem);
}

void ShaderState_SetTexCube (ShaderState* self, cstr name, TexCube* x) {
  TexCube_Acquire(x);
  Elem elem = { ElemType_TexCube, Shader_GetVariable(self->shader, name) };
  elem.data.asTexCube = x;
  ArrayList_Append(self->elems, elem);
}

void ShaderState_Start (ShaderState* self) {
  Shader_Start(self->shader);
  ArrayList_ForEach(self->elems, Elem, e) {
    switch (e->type) {
      case ElemType_Float:
        glUniform1f(e->index, e->data.asFloat);
        break;

      case ElemType_Float2:
        glUniform2f(e->index,
          e->data.asFloat2.x,
          e->data.asFloat2.y);
        break;

      case ElemType_Float3:
        glUniform3f(e->index,
          e->data.asFloat3.x,
          e->data.asFloat3.y,
          e->data.asFloat3.z);
        break;

      case ElemType_Float4:
        glUniform4f(e->index,
          e->data.asFloat4.x,
          e->data.asFloat4.y,
          e->data.asFloat4.z,
          e->data.asFloat4.w);
        break;

      case ElemType_Int:
        glUniform1i(e->index, e->data.asInt);
        break;

      case ElemType_Matrix:
        Shader_ISetMatrix(e->index, e->data.asMatrix);
        break;

      case ElemType_Tex1D:
        Shader_ISetTex1D(e->index, e->data.asTex1D);
        break;

      case ElemType_Tex2D:
        Shader_ISetTex2D(e->index, e->data.asTex2D);
        break;

      case ElemType_Tex3D:
        Shader_ISetTex3D(e->index, e->data.asTex3D);
        break;

      case ElemType_TexCube:
        Shader_ISetTexCube(e->index, e->data.asTexCube);
        break;

      default:
        Fatal("ShaderState_Start: Encountered invalid opcode");
    }
  }
}

void ShaderState_Stop (ShaderState* self) {
  Shader_Stop(self->shader);
}

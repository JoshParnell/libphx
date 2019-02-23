#include "DataFormat.h"
#include "Draw.h"
#include "PhxMemory.h"
#include "Mesh.h"
#include "PixelFormat.h"
#include "RenderState.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "PhxMath.h"
#include "Tex2D.h"
#include "TexFormat.h"
#include "Vec4.h"
#include "Vertex.h"

#define USE_CPU 0

#if USE_CPU

#include "BSP.h"
#include "Ray.h"
#include "RNG.h"

/* TODO : This is broken...BSP returning negatives & NaNs...?
 *        Probably would be better if we could do it this way, though, since
 *        we should see scalability gains due to BSP acceleration. This can
 *        also be made physically-accurate, unlike the GPU version. */
void Mesh_ComputeAO (Mesh* self, float radius) {
  int vertexCount = Mesh_GetVertexCount(self);
  Vertex* vertexData = Mesh_GetVertexData(self);

  BSP* bsp = BSP_Create(self);
  RNG* rng = RNG_Create(vertexCount);

  const int kSamples = 128;
  Ray ray;
  ray.tMin = 0.0f;
  ray.tMax = 1e6f;
  float t;
  for (int i = 0; i < vertexCount; ++i) {
    Vertex* v = vertexData + i;
    ray.p = Vec3f_Add(v->p, Vec3f_Muls(v->n, 0.01f));
    float occlusion = 0;
    for (int j = 0; j < kSamples; ++j) {
      /* NOTE : Ignoring the projected area differential factor here. Could
       *        easily importance sample if necessary:
       *          Z = rng:getUniform();
       *          D = sqrt(1.0 - z*z) * Normalize(Reject(D, N)) */
      RNG_GetDir3(rng, &ray.dir);
      ray.dir = Vec3f_Muls(ray.dir, Sign(Vec3f_Dot(ray.dir, v->n)));
      if (BSP_IntersectRay(bsp, &ray, &t))
        occlusion += Exp(-t / radius);
    }
    v->uv.x = 1.0f - occlusion / (float)kSamples;
  }

  BSP_Free(bsp);
  RNG_Free(rng);
}

#else

/* TODO : Needs to be asymptotically faster. N^2 currently. */
void Mesh_ComputeAO (Mesh* self, float radius) {
  int indexCount = Mesh_GetIndexCount(self);
  int vertexCount = Mesh_GetVertexCount(self);
  int* indexData = Mesh_GetIndexData(self);
  Vertex* vertexData = Mesh_GetVertexData(self);

  int sDim = (int)Ceil(Sqrt((double)(indexCount / 3)));
  int vDim = (int)Ceil(Sqrt((double)vertexCount));
  int surfels = sDim * sDim;
  int vertices = vDim * vDim;
  int bufSize = Max(surfels, vertices);

  Vec4f* pointBuffer = MemNewArray(Vec4f, bufSize);
  Vec4f* normalBuffer = MemNewArray(Vec4f, bufSize);

  /* Fill buffers with surfel data. */
  MemZero(pointBuffer, sizeof(Vec4f) * bufSize);
  MemZero(normalBuffer, sizeof(Vec4f) * bufSize);
  for (int i = 0; i < indexCount; i += 3) {
    Vertex const* v1 = vertexData + indexData[i + 0];
    Vertex const* v2 = vertexData + indexData[i + 1];
    Vertex const* v3 = vertexData + indexData[i + 2];

    Vec3f normal = Vec3f_Cross(Vec3f_Sub(v3->p, v1->p), Vec3f_Sub(v2->p, v1->p));
    float length = Vec3f_Length(normal);
    float area = 0.5f * length / Pi;
    if (Abs(length) > 1e-6)
      Vec3f_IDivs(&normal, length);
    else
      normal = Vec3f_Create(1, 0, 0);

    /* Face center. */ {
      Vec3f center = Vec3f_Divs(Vec3f_Add(v1->p, Vec3f_Add(v2->p, v3->p)), 3.0f);
      pointBuffer[i / 3] = Vec4f_Create(center.x, center.y, center.z, area);
    }

    normalBuffer[i / 3] = Vec4f_Create(normal.x, normal.y, normal.z, 0);
  }

  Tex2D* texSPoints = Tex2D_Create(sDim, sDim, TexFormat_RGBA32F);
  Tex2D* texSNormals = Tex2D_Create(sDim, sDim, TexFormat_RGBA32F);
  Tex2D_SetData(texSPoints, pointBuffer, PixelFormat_RGBA, DataFormat_Float);
  Tex2D_SetData(texSNormals, normalBuffer, PixelFormat_RGBA, DataFormat_Float);

  /* Fill buffers with vertex data. */
  MemZero(pointBuffer, sizeof(Vec4f) * bufSize);
  MemZero(normalBuffer, sizeof(Vec4f) * bufSize);
  for (int i = 0; i < vertexCount; ++i) {
    Vertex const* v = vertexData + i;
    pointBuffer[i] = Vec4f_Create(v->p.x, v->p.y, v->p.z, 0);
    normalBuffer[i] = Vec4f_Create(v->n.x, v->n.y, v->n.z, 0);
  }

  Tex2D* texVPoints = Tex2D_Create(vDim, vDim, TexFormat_RGBA32F);
  Tex2D* texVNormals = Tex2D_Create(vDim, vDim, TexFormat_RGBA32F);
  Tex2D_SetData(texVPoints, pointBuffer, PixelFormat_RGBA, DataFormat_Float);
  Tex2D_SetData(texVNormals, normalBuffer, PixelFormat_RGBA, DataFormat_Float);
  MemFree(pointBuffer);
  MemFree(normalBuffer);

  Tex2D* texOutput = Tex2D_Create(vDim, vDim, TexFormat_R32F);

  static Shader* shader = 0;
  if (!shader)
    shader = Shader_Load("vertex/identity", "fragment/compute/occlusion");

  RenderState_PushAllDefaults();
  RenderTarget_PushTex2D(texOutput);
    Shader_Start(shader);
    Shader_SetInt("sDim", sDim);
    Shader_SetFloat("radius", radius);
    Shader_SetTex2D("sPointBuffer", texSPoints);
    Shader_SetTex2D("sNormalBuffer", texSNormals);
    Shader_SetTex2D("vPointBuffer", texVPoints);
    Shader_SetTex2D("vNormalBuffer", texVNormals);
    Draw_Rect(-1, -1, 2, 2);
    Shader_Stop(shader);
  RenderTarget_Pop();
  RenderState_PopAll();

  /* Write result to mesh. */ {
    float* result = MemNewArray(float, vDim * vDim);
    Tex2D_GetData(texOutput, result, PixelFormat_Red, DataFormat_Float);
    for (int i = 0; i < vertexCount; ++i)
      vertexData[i].uv.x = result[i];
    MemFree(result);
  }

  Tex2D_Free(texOutput);
  Tex2D_Free(texSPoints);
  Tex2D_Free(texSNormals);
  Tex2D_Free(texVPoints);
  Tex2D_Free(texVNormals);
}

#endif

/* SDF-Based Occlusion. */
void Mesh_ComputeOcclusion (Mesh* self, Tex3D* sdf, float radius) {
  int vertexCount = Mesh_GetVertexCount(self);
  Vertex* vertexData = Mesh_GetVertexData(self);

  int vDim = (int)Ceil(Sqrt((double)vertexCount));
  Tex2D* texPoints = Tex2D_Create(vDim, vDim, TexFormat_RGBA32F);
  Tex2D* texOutput = Tex2D_Create(vDim, vDim, TexFormat_R32F);

  /* Fill input texture with vertex positions. */ {
    Vec3f* pointBuffer = MemNewArray(Vec3f, vDim * vDim);
    for (int i = 0; i < vertexCount; ++i)
      pointBuffer[i] = vertexData[i].p;
    Tex2D_SetData(texPoints, pointBuffer, PixelFormat_RGB, DataFormat_Float);
    MemFree(pointBuffer);
  }

  static Shader* shader = 0;
  if (!shader)
    shader = Shader_Load("vertex/identity", "fragment/compute/occlusion_sdf");

  /* Compute. */ {
    RenderState_PushAllDefaults();
    RenderTarget_PushTex2D(texOutput);
      Shader_Start(shader);
      Shader_SetFloat("radius", radius);
      Shader_SetTex2D("points", texPoints);
      Shader_SetTex3D("sdf", sdf);
      Draw_Rect(-1, -1, 2, 2);
      Shader_Stop(shader);
    RenderTarget_Pop();
    RenderState_PopAll();
  }

  /* Extract results and copy to mesh vertices. */ {
    float* result = MemNewArray(float, vDim * vDim);
    Tex2D_GetData(texOutput, result, PixelFormat_Red, DataFormat_Float);
    for (int i = 0; i < vertexCount; ++i)
      vertexData[i].uv.x = result[i];
    MemFree(result);
  }

  Tex2D_Free(texPoints);
  Tex2D_Free(texOutput);
}

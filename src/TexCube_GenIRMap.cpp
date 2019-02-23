#include "CubeFace.h"
#include "DataFormat.h"
#include "Draw.h"
#include "PhxMemory.h"
#include "PixelFormat.h"
#include "RenderTarget.h"
#include "RNG.h"
#include "Shader.h"
#include "Tex2D.h"
#include "TexCube.h"
#include "TexFilter.h"
#include "TexFormat.h"
#include "Vec2.h"

/* TODO : Make this cleaner and faster. */
/* TODO : This is very specific functionality (GGX mips). Where does this kind
 *        of functionality really belong? Not PHX core but not Lua.. */

TexCube* TexCube_GenIRMap (TexCube* self, int sampleCount) {
  int size = TexCube_GetSize(self);
  TexFormat format = TexCube_GetFormat(self);
  TexCube* result = TexCube_Create(size, format);

  /* Copy 0th-level data directly, as it should be identical. Doing so in
   * shader may result in subtle differences. */ {
    int components = TexFormat_Components(format);
    PixelFormat pf =
      components == 4 ? PixelFormat_RGBA :
      components == 3 ? PixelFormat_RGB  :
      components == 2 ? PixelFormat_RG   :
                        PixelFormat_Red;
    DataFormat df = DataFormat_Float;

    void* buffer = MemAlloc(size * size * sizeof(float) * components);
    for (int i = 0; i < 6; ++i) {
      TexCube_GetData(self, buffer, CubeFace_Get(i), 0, pf, df);
      TexCube_SetData(result, buffer, CubeFace_Get(i), 0, pf, df);
    }
    TexCube_GenMipmap(result);
    MemFree(buffer);
  }

  static Shader* shader = 0;
  if (!shader)
    shader = Shader_Load("vertex/identity", "fragment/compute/irmap");

  CubeFace const face[6] = {
    CubeFace_PX, CubeFace_NX, CubeFace_PY,
    CubeFace_NY, CubeFace_PZ, CubeFace_NZ,
  };

  Vec3f const look[6] = {
    {  1,  0,  0 }, { -1,  0,  0 }, {  0,  1,  0 },
    {  0, -1,  0 }, {  0,  0,  1 }, {  0,  0, -1 },
  };

  Vec3f const up[6] = {
    {  0,  1,  0 }, {  0,  1,  0 }, {  0,  0, -1 },
    {  0,  0,  1 }, {  0,  1,  0 }, {  0,  1,  0 },
  };

  RNG* rng = RNG_FromTime();

  int levels = 0;
  for (int i = size; i > 0; i /= 2)
    levels++;

  Shader_Start(shader);
  int level = 0;
  while (size > 1) {
    size /= 2;
    level += 1;

    double ggxWidth = (double)level / (double)levels;
    ggxWidth *= ggxWidth;

    Vec2f* sampleBuffer = MemNewArray(Vec2f, sampleCount);
    Tex2D* sampleTex = Tex2D_Create(sampleCount, 1, TexFormat_RG16F);
    /* Populate the sample buffer with a fresh set of weighted directions */ {
      for (int i = 0; i < sampleCount; ++i) {
        double e1 = RNG_GetUniform(rng);
        double e2 = RNG_GetUniform(rng);
        double pitch = Atan(ggxWidth * Sqrt(e1), Sqrt(1.0 - e1));
        double yaw = Tau * e2;
        sampleBuffer[i] = Vec2f_Create((float)pitch, (float)yaw);
      }
      Tex2D_SetData(sampleTex, sampleBuffer, PixelFormat_RG, DataFormat_Float);
    }

    float angle = (float)level / (float)(levels - 1);
    angle = angle * angle;

    Shader_SetFloat("angle", angle);
    Shader_SetTexCube("src", self);
    Shader_SetTex2D("sampleBuffer", sampleTex);
    Shader_SetInt("samples", sampleCount);

    for (int i = 0; i < 6; ++i) {
      CubeFace thisFace = face[i];
      Vec3f thisLook = look[i];
      Vec3f thisUp = up[i];
      RenderTarget_Push(size, size);
      RenderTarget_BindTexCubeLevel(result, thisFace, level);
      Shader_SetFloat3("cubeLook", thisLook.x, thisLook.y, thisLook.z);
      Shader_SetFloat3("cubeUp", thisUp.x, thisUp.y, thisUp.z);
      Draw_Rect(-1, -1, 2, 2);
      RenderTarget_Pop();
    }

    MemFree(sampleBuffer);
    Tex2D_Free(sampleTex);
  }

  RNG_Free(rng);
  Shader_Stop(shader);
  TexCube_SetMagFilter(result, TexFilter_Linear);
  TexCube_SetMinFilter(result, TexFilter_LinearMipLinear);
  return result;
}

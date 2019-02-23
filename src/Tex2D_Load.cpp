#include "Common.h"
#include "Tex2D.h"

#pragma warning(push)
#pragma warning(disable:4242)
#pragma warning(disable:4244)
//https://github.com/nothings/stb/issues/334
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#pragma warning(pop)

uchar* Tex2D_LoadRaw (cstr path, int* sx, int* sy, int* components) {
  uchar* data = stbi_load(path, sx, sy, components, 0);
  if (!data)
    Fatal("Failed to load image from '%s'", path);
  return data;
}

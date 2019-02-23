#include "Common.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

bool Tex2D_Save_Png (cstr path, int sx, int sy, int components, uchar* data) {
  int stride = components * sx;
  int result = stbi_write_png(path, sx, sy, components, data, stride);
  return result != 0;
}

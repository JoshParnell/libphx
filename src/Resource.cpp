#include "File.h"
#include "PhxMemory.h"
#include "Resource.h"
#include "PhxString.h"

struct PathElem {
  cstr format;
  PathElem* next;
};

static PathElem* paths[ResourceType_COUNT] = { 0 };

inline static cstr Resource_Resolve (ResourceType type, cstr name, bool failhard) {
  static char buffer[256];
  for (PathElem* elem = paths[type]; elem; elem = elem->next) {
    int res = snprintf(buffer, sizeof(buffer), elem->format, name);
    if (res > 0 && res < (int)sizeof(buffer)) {
      if (File_Exists(buffer))
        return buffer;
    }
  }

  /* Attempt to fall back to literal path if no path rule worked. */
  if (name && File_Exists(name))
    return name;

  if (failhard)
    Fatal("Resource_Resolve: Failed to find %s <%s>",
      ResourceType_ToString(type), name);

  return 0;
}

void Resource_AddPath (ResourceType type, cstr format) {
  PathElem* self = MemNew(PathElem);
  self->format = StrDup(format);
  self->next = paths[type];
  paths[type] = self;
}

bool Resource_Exists (ResourceType type, cstr name) {
  return Resource_Resolve(type, name, false) != 0;
}

cstr Resource_GetPath (ResourceType type, cstr name) {
  return Resource_Resolve(type, name, true);
}

Bytes* Resource_LoadBytes (ResourceType type, cstr name) {
  cstr path = Resource_Resolve(type, name, true);
  Bytes* data = File_ReadBytes(path);
  if (!data)
    Fatal("Resource_LoadBytes: Failed to load %s <%s> at <%s>",
      ResourceType_ToString(type), name, path);
  return data;
}

cstr Resource_LoadCstr (ResourceType type, cstr name) {
  cstr path = Resource_Resolve(type, name, true);
  cstr data = File_ReadCstr(path);
  if (!data)
    Fatal("Resource_LoadCstr: Failed to load %s <%s> at <%s>",
      ResourceType_ToString(type), name, path);
  return data;
}

void Resource_Init () {
#ifndef BUILD_DISTRIBUTABLE
  Resource_AddPath(ResourceType_Font,    "../shared/res/font/%s.ttf");
  Resource_AddPath(ResourceType_Font,    "../shared/res/font/%s.otf");
  Resource_AddPath(ResourceType_Mesh,    "../shared/res/mesh/%s.bin");
  Resource_AddPath(ResourceType_Mesh,    "../shared/res/mesh/%s.obj");
  Resource_AddPath(ResourceType_Other,   "../shared/res/%s");
  Resource_AddPath(ResourceType_Script,  "../shared/res/script/%s.lua");
  Resource_AddPath(ResourceType_Shader,  "../shared/res/shader/%s.glsl");
  Resource_AddPath(ResourceType_Sound,   "../shared/res/sound/%s.mp3");
  Resource_AddPath(ResourceType_Sound,   "../shared/res/sound/%s.ogg");
  Resource_AddPath(ResourceType_Sound,   "../shared/res/sound/%s.ogx");
  Resource_AddPath(ResourceType_Sound,   "../shared/res/sound/%s.wav");
  Resource_AddPath(ResourceType_Tex1D,   "../shared/res/tex1d/%s.bin");
  Resource_AddPath(ResourceType_Tex2D,   "../shared/res/tex2d/%s.jpg");
  Resource_AddPath(ResourceType_Tex2D,   "../shared/res/tex2d/%s.png");
  Resource_AddPath(ResourceType_Tex3D,   "../shared/res/tex3d/%s.bin");
  Resource_AddPath(ResourceType_TexCube, "../shared/res/texcube/%s");
#endif

  Resource_AddPath(ResourceType_Font,    "./res/font/%s.ttf");
  Resource_AddPath(ResourceType_Font,    "./res/font/%s.otf");
  Resource_AddPath(ResourceType_Mesh,    "./res/mesh/%s.bin");
  Resource_AddPath(ResourceType_Mesh,    "./res/mesh/%s.obj");
  Resource_AddPath(ResourceType_Other,   "./res/%s");
  Resource_AddPath(ResourceType_Script,  "./res/script/%s.lua");
  Resource_AddPath(ResourceType_Shader,  "./res/shader/%s.glsl");
  Resource_AddPath(ResourceType_Sound,   "./res/sound/%s.mp3");
  Resource_AddPath(ResourceType_Sound,   "./res/sound/%s.ogg");
  Resource_AddPath(ResourceType_Sound,   "./res/sound/%s.ogx");
  Resource_AddPath(ResourceType_Sound,   "./res/sound/%s.wav");
  Resource_AddPath(ResourceType_Tex1D,   "./res/tex1d/%s.bin");
  Resource_AddPath(ResourceType_Tex2D,   "./res/tex2d/%s.jpg");
  Resource_AddPath(ResourceType_Tex2D,   "./res/tex2d/%s.png");
  Resource_AddPath(ResourceType_Tex3D,   "./res/tex3d/%s.bin");
  Resource_AddPath(ResourceType_TexCube, "./res/texcube/%s");

  Resource_AddPath(ResourceType_Font,    "%s.ttf");
  Resource_AddPath(ResourceType_Font,    "%s.otf");
  Resource_AddPath(ResourceType_Mesh,    "%s.bin");
  Resource_AddPath(ResourceType_Mesh,    "%s.obj");
  Resource_AddPath(ResourceType_Other,   "%s");
  Resource_AddPath(ResourceType_Script,  "%s.lua");
  Resource_AddPath(ResourceType_Shader,  "%s.glsl");
  Resource_AddPath(ResourceType_Sound,   "%s.mp3");
  Resource_AddPath(ResourceType_Sound,   "%s.ogg");
  Resource_AddPath(ResourceType_Sound,   "%s.ogx");
  Resource_AddPath(ResourceType_Sound,   "%s.wav");
  Resource_AddPath(ResourceType_Tex1D,   "%s.bin");
  Resource_AddPath(ResourceType_Tex2D,   "%s.jpg");
  Resource_AddPath(ResourceType_Tex2D,   "%s.png");
  Resource_AddPath(ResourceType_Tex3D,   "%s.bin");
  Resource_AddPath(ResourceType_TexCube, "%s");
}

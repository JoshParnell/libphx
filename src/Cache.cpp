#include "Cache.h"
#include "PhxString.h"
#include "Shader.h"
#include "StrMap.h"

struct Cache {
  StrMap* shaderMap;
} static self = { 0 };

static void CacheFreeShader (cstr, void* self) {
  Shader_Free((Shader*)self);
}

void Cache_Init () {
  self.shaderMap = StrMap_Create(16);
}

void Cache_Clear () {
  StrMap_FreeEx(self.shaderMap, CacheFreeShader);
  Cache_Init();
}

Shader* Cache_GetShader (cstr vs, cstr fs) {
  /* TODO : Get rid of memory allocation here. */
  cstr key = StrFormat("%s|%s", vs, fs);
  Shader* value = (Shader*)StrMap_Get(self.shaderMap, key);
  if (!value) {
    value = Shader_Load(vs, fs);
    StrMap_Set(self.shaderMap, key, value);
  }

  StrFree(key);
  return value;
}

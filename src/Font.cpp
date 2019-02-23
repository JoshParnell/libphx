#include "BlendMode.h"
#include "DataFormat.h"
#include "Draw.h"
#include "Font.h"
#include "HashMap.h"
#include "PhxMemory.h"
#include "OpenGL.h"
#include "Profiler.h"
#include "RenderState.h"
#include "RefCounted.h"
#include "Resource.h"
#include "PhxMath.h"
#include "Shader.h"
#include "Tex2D.h"
#include "PixelFormat.h"
#include "TexFormat.h"
#include "Vec2.h"
#include "Vec4.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H

/* TODO : Re-implement UTF-8 support */
/* TODO : Atlas instead of individual textures. */

/* NOTE : Gamma of 1.8 recommended by FreeType */
const float kGamma = 1.8f;
const float kRcpGamma = 1.0f / kGamma;

struct Glyph {
  int index;
  Tex2D* tex;
  int x0, y0, x1, y1;
  int sx, sy;
  int advance;
};

struct Font {
  RefCounted;
  FT_Face handle;
  HashMap* glyphs;
  Glyph* glyphsAscii[256];
};

static FT_Library ft = 0;

static Glyph* Font_GetGlyph (Font* self, uint32 codepoint) {
  if (codepoint < 256 && self->glyphsAscii[codepoint])
    return self->glyphsAscii[codepoint];

  Glyph* g = (Glyph*)HashMap_Get(self->glyphs, &codepoint);
  if (g)
    return g;

  FT_Face face = self->handle;
  int glyph = FT_Get_Char_Index(face, codepoint);
  if (glyph == 0)
    return 0;
  if (FT_Load_Glyph(face, glyph, FT_LOAD_FORCE_AUTOHINT | FT_LOAD_RENDER))
    return 0;

  FT_Bitmap const* bitmap = &face->glyph->bitmap;
  uchar const* pBitmap = bitmap->buffer;

  /* Create a new glyph and fill out metrics. */ {
    g = MemNew(Glyph);
    g->index = glyph;
    g->x0 = face->glyph->bitmap_left;
    g->y0 = -face->glyph->bitmap_top;
    g->sx = bitmap->width;
    g->sy = bitmap->rows;
    g->x1 = g->x0 + g->sx;
    g->y1 = g->y0 + g->sy;
    g->advance = face->glyph->advance.x >> 6;
  }

  Vec4f* buffer = MemNewArray(Vec4f, g->sx * g->sy);

  /* Copy rendered bitmap into buffer. */ {
    Vec4f* pBuffer = buffer;
    for (uint dy = 0; dy < bitmap->rows; ++dy) {
      for (uint dx = 0; dx < bitmap->width; ++dx) {
        float a = Pow((float)(pBitmap[dx]) / 255.0f, kRcpGamma);
        *pBuffer++ = Vec4f_Create(1.0f, 1.0f, 1.0f, a);
      }
      pBitmap += bitmap->pitch;
    }
  }

  /* Upload to texture. */ {
    g->tex = Tex2D_Create(g->sx, g->sy, TexFormat_RGBA8);
    Tex2D_SetData(g->tex, buffer, PixelFormat_RGBA, DataFormat_Float);
  }

  MemFree(buffer);

  /* Add to glyph cache. */
  if (codepoint < 256)
    self->glyphsAscii[codepoint] = g;
  else
    HashMap_Set(self->glyphs, &codepoint, g);
  return g;
}

inline static int Font_GetKerning (Font* self, int a, int b) {
  FT_Vector kern;
  FT_Get_Kerning(self->handle, a, b, FT_KERNING_DEFAULT, &kern);
  return kern.x >> 6;
}

Font* Font_Load (cstr name, int size) {
  if (!ft)
    FT_Init_FreeType(&ft);

  cstr path = Resource_GetPath(ResourceType_Font, name);
  Font* self = MemNew(Font);
  RefCounted_Init(self);

  if (FT_New_Face(ft, path, 0, &self->handle))
    Fatal("Font_Load: Failed to load font <%s> at <%s>", name, path);
  FT_Set_Pixel_Sizes(self->handle, 0, size);

  MemZero(self->glyphsAscii, sizeof(self->glyphsAscii));
  self->glyphs = HashMap_Create(sizeof(uint32), 16);
  return self;
}

void Font_Acquire (Font* self) {
  RefCounted_Acquire(self);
}

void Font_Free (Font* self) {
  RefCounted_Free(self) {
    /* TODO : Free glyphs! */
    FT_Done_Face(self->handle);
    MemFree(self);
  }
}

void Font_Draw (
  Font* self, cstr text,
  float x, float y,
  float r, float g, float b, float a)
{
  FRAME_BEGIN;
  int glyphLast = 0;
  uint32 codepoint = *text++;
  x = Floor(x);
  y = Floor(y);
  RenderState_PushBlendMode(BlendMode_Alpha);
  Draw_Color(r, g, b, a);

  while (codepoint) {
    Glyph* glyph = Font_GetGlyph(self, codepoint);
    if (glyph) {
      if (glyphLast)
        x += Font_GetKerning(self, glyphLast, glyph->index);
      float x0 = (float)(x + glyph->x0);
      float y0 = (float)(y + glyph->y0);
      float x1 = (float)(x + glyph->x1);
      float y1 = (float)(y + glyph->y1);
      Tex2D_DrawEx(glyph->tex, x0, y0, x1, y1, 0, 0, 1, 1);
      x += glyph->advance;
      glyphLast = glyph->index;
    } else {
      glyphLast = 0;
    }
    codepoint = *text++;
  }

  Draw_Color(1, 1, 1, 1);
  RenderState_PopBlendMode();
  FRAME_END;
}

void Font_DrawShaded (Font* self, cstr text, float x, float y) {
  FRAME_BEGIN;
  int glyphLast = 0;
  uint32 codepoint = *text++;
  x = Floor(x);
  y = Floor(y);

  while (codepoint) {
    Glyph* glyph = Font_GetGlyph(self, codepoint);
    if (glyph) {
      if (glyphLast)
        x += Font_GetKerning(self, glyphLast, glyph->index);
      float x0 = (float)(x + glyph->x0);
      float y0 = (float)(y + glyph->y0);
      float x1 = (float)(x + glyph->x1);
      float y1 = (float)(y + glyph->y1);
      Shader_SetTex2D("glyph", glyph->tex);
      Tex2D_DrawEx(glyph->tex, x0, y0, x1, y1, 0, 0, 1, 1);
      x += glyph->advance;
      glyphLast = glyph->index;
    } else {
      glyphLast = 0;
    }
    codepoint = *text++;
  }

  FRAME_END;
}

int Font_GetLineHeight (Font* self) {
  return self->handle->size->metrics.height >> 6;
}

void Font_GetSize (Font* self, Vec4i* out, cstr text) {
  FRAME_BEGIN;
  int x = 0, y = 0;
  Vec2i lower = { INT_MAX, INT_MAX };
  Vec2i upper = { INT_MIN, INT_MIN };

  int glyphLast = 0;
  uint32 codepoint = *text++;
  if (!codepoint) {
    *out = Vec4i_Create(0, 0, 0, 0);
    return;
  }

  while (codepoint) {
    Glyph* glyph = Font_GetGlyph(self, codepoint);
    if (glyph) {
      if (glyphLast)
        x += Font_GetKerning(self, glyphLast, glyph->index);
      lower.x = Min(lower.x, x + glyph->x0);
      lower.y = Min(lower.y, y + glyph->y0);
      upper.x = Max(upper.x, x + glyph->x1);
      upper.y = Max(upper.y, y + glyph->y1);
      x += glyph->advance;
      glyphLast = glyph->index;
    } else {
      glyphLast = 0;
    }
    codepoint = *text++;
  }

  *out = Vec4i_Create(lower.x, lower.y, upper.x - lower.x, upper.y - lower.y);
  FRAME_END;
}

/* NOTE : The height returned here is the maximal *ascender* height for the
 *        string. This allows easy centering of text while still allowing
 *        descending characters to look correct.
 *
 *        To correctly center text, first compute bounds via this function,
 *        then draw it at:
 *
 *           pos.x - (size.x - bound.x) / 2
 *           pos.y - (size.y + bound.y) / 2
 */

void Font_GetSize2 (Font* self, Vec2i* out, cstr text) {
  FRAME_BEGIN;
  out->x = 0;
  out->y = 0;

  int glyphLast = 0;
  uint32 codepoint = *text++;
  while (codepoint) {
    Glyph* glyph = Font_GetGlyph(self, codepoint);
    if (glyph) {
      if (glyphLast)
        out->x += Font_GetKerning(self, glyphLast, glyph->index);
      out->x += glyph->advance;
      out->y = Max(out->y, -glyph->y0 + 1);
      glyphLast = glyph->index;
    } else {
      glyphLast = 0;
    }
    codepoint = *text++;
  }

  FRAME_END;
}

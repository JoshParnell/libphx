#include "BlendMode.h"
#include "ClipRect.h"
#include "Draw.h"
#include "Font.h"
#include "MemPool.h"
#include "RenderState.h"
#include "Shader.h"
#include "Tex2D.h"
#include "UIRenderer.h"
#include "Vec2.h"
#include "Vec4.h"
#include "Viewport.h"

struct UIRendererImage {
  UIRendererImage* next;
  Tex2D* image;
  Vec2f pos;
  Vec2f size;
};

struct UIRendererPanel {
  UIRendererPanel* next;
  Vec2f pos;
  Vec2f size;
  Vec4f color;
  float bevel;
  float innerAlpha;
};

struct UIRendererRect {
  UIRendererRect* next;
  Vec2f pos;
  Vec2f size;
  Vec4f color;
  bool outline;
};

struct UIRendererText {
  UIRendererText* next;
  Font* font;
  cstr text;
  Vec2f pos;
  Vec4f color;
};

struct UIRendererLayer {
  UIRendererLayer* parent;
  UIRendererLayer* next;
  UIRendererLayer* children;

  UIRendererImage* imageList;
  UIRendererPanel* panelList;
  UIRendererRect* rectList;
  UIRendererText* textList;

  Vec2f pos;
  Vec2f size;
  bool clip;
};

struct UIRenderer {
  UIRendererLayer* root;
  UIRendererLayer* layer;

  MemPool* layerPool;
  MemPool* imagePool;
  MemPool* panelPool;
  MemPool* rectPool;
  MemPool* textPool;
} static self = { 0 };

static void UIRenderer_Init () {
  static bool init = false;
  if (init) return;
  init = true;

  self.root = 0;
  self.layer = 0;

  self.layerPool = MemPool_CreateAuto(sizeof(UIRendererLayer));
  self.imagePool = MemPool_CreateAuto(sizeof(UIRendererImage));
  self.panelPool = MemPool_CreateAuto(sizeof(UIRendererPanel));
  self.rectPool = MemPool_CreateAuto(sizeof(UIRendererRect));
  self.textPool = MemPool_CreateAuto(sizeof(UIRendererText));
}

void UIRenderer_Begin () {
  UIRenderer_Init();

  self.root = 0;
  self.layer = 0;

  MemPool_Clear(self.layerPool);
  MemPool_Clear(self.imagePool);
  MemPool_Clear(self.panelPool);
  MemPool_Clear(self.rectPool);
  MemPool_Clear(self.textPool);

  Vec2i vp; Viewport_GetSize(&vp);
  UIRenderer_BeginLayer(0, 0, vp.x, vp.y, true);
  self.root = self.layer;
}

void UIRenderer_End () {
  UIRenderer_EndLayer();
}

static void UIRenderer_DrawLayer (UIRendererLayer const* self) {
  if (self->clip)
    ClipRect_PushCombined(self->pos.x, self->pos.y, self->size.x, self->size.y);

  if (self->panelList) {
    static Shader* shader = 0;
    if (!shader)
      shader = Shader_Load("vertex/ui", "fragment/ui/panel");

    const float pad = 64.0f;
    Shader_Start(shader);
    Shader_SetFloat("padding", pad);

    for (UIRendererPanel const* e = self->panelList; e; e = e->next) {
      float x = e->pos.x - pad;
      float y = e->pos.y - pad;
      float sx = e->size.x + 2.0f * pad;
      float sy = e->size.y + 2.0f * pad;

      Shader_SetFloat("innerAlpha", e->innerAlpha);
      Shader_SetFloat("bevel", e->bevel);
      Shader_SetFloat2("size", sx, sy);
      Shader_SetFloat4("color", UNPACK4(e->color));
      Draw_Rect(x, y, sx, sy);
    }

    Shader_Stop(shader);
  }

  for (UIRendererImage const* e = self->imageList; e; e = e->next) {
    Tex2D_Draw(e->image, UNPACK2(e->pos), UNPACK2(e->size));
  }

  for (UIRendererRect const* e = self->rectList; e; e = e->next) {
    Draw_Color(UNPACK4(e->color));
    if (e->outline)
      Draw_Border(1.0f, UNPACK2(e->pos), UNPACK2(e->size));
    else
      Draw_Rect(UNPACK2(e->pos), UNPACK2(e->size));
  }

  for (UIRendererText const* e = self->textList; e; e = e->next)
    Font_Draw(e->font, e->text, UNPACK2(e->pos), UNPACK4(e->color));

  for (UIRendererLayer const* e = self->children; e; e = e->next)
    UIRenderer_DrawLayer(e);

  if (self->clip)
    ClipRect_Pop();
}

void UIRenderer_Draw () {
  RenderState_PushBlendMode(BlendMode_Alpha);
  UIRenderer_DrawLayer(self.root);
  RenderState_PopBlendMode();
}

void UIRenderer_BeginLayer (float x, float y, float sx, float sy, bool clip) {
  UIRendererLayer* layer = (UIRendererLayer*)MemPool_Alloc(self.layerPool);
  layer->parent = self.layer;
  layer->next = 0;
  layer->children = 0;

  layer->pos = Vec2f_Create(x, y);
  layer->size = Vec2f_Create(sx, sy);
  layer->clip = clip;

  layer->imageList = 0;
  layer->panelList = 0;
  layer->rectList = 0;
  layer->textList = 0;
  self.layer = layer;
}

void UIRenderer_EndLayer () {
  if (self.layer->parent) {
    self.layer->next = self.layer->parent->children;
    self.layer->parent->children = self.layer;
  }
  self.layer = self.layer->parent;
}

void UIRenderer_Image (
  Tex2D* image,
  float x, float y,
  float sx, float sy)
{
  UIRendererImage* e = (UIRendererImage*)MemPool_Alloc(self.imagePool);
  e->next = self.layer->imageList;
  e->image = image;
  e->pos = Vec2f_Create(x, y);
  e->size = Vec2f_Create(sx, sy);
  self.layer->imageList = e;
}

void UIRenderer_Panel (
  float x, float y,
  float sx, float sy,
  float r, float g, float b, float a,
  float bevel,
  float innerAlpha)
{
  UIRendererPanel* e = (UIRendererPanel*)MemPool_Alloc(self.panelPool);
  e->next = self.layer->panelList;
  e->pos = Vec2f_Create(x, y);
  e->size = Vec2f_Create(sx, sy);
  e->color = Vec4f_Create(r, g, b, a);
  e->bevel = bevel;
  e->innerAlpha = innerAlpha;
  self.layer->panelList = e;
}

void UIRenderer_Rect (
  float x, float y,
  float sx, float sy,
  float r, float g, float b, float a,
  bool outline)
{
  UIRendererRect* e = (UIRendererRect*)MemPool_Alloc(self.rectPool);
  e->next = self.layer->rectList;
  e->pos = Vec2f_Create(x, y);
  e->size = Vec2f_Create(sx, sy);
  e->color = Vec4f_Create(r, g, b, a);
  e->outline = outline;
  self.layer->rectList = e;
}

void UIRenderer_Text (
  Font* font, cstr text,
  float x, float y,
  float r, float g, float b, float a)
{
  UIRendererText* e = (UIRendererText*)MemPool_Alloc(self.textPool);
  e->next = self.layer->textList;
  e->font = font;
  e->text = text;
  e->pos = Vec2f_Create(x, y);
  e->color = Vec4f_Create(r, g, b, a);
  self.layer->textList = e;
}

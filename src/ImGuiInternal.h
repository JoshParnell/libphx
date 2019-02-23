/* -------------------------------------------------------------------------- */

const int FocusType_Mouse    = 0;
const int FocusType_Keyboard = 1;
const int FocusType_Scroll   = 2;
const int FocusType_SIZE     = 3;

struct ImGuiClipRect {
  ImGuiClipRect* prev;
  Vec2f p1;
  Vec2f p2;
};

struct ImGuiCursor {
  ImGuiCursor* prev;
  Vec2f pos;
};

struct ImGuiLine {
  ImGuiLine* next;
  Vec4f color;
  Vec2f p1;
  Vec2f p2;
};

struct ImGuiPanel {
  ImGuiPanel* next;
  Vec4f color;
  Vec2f pos;
  Vec2f size;
  float innerAlpha;
  float bevel;
};

struct ImGuiRect {
  ImGuiRect* next;
  Vec4f color;
  Vec2f pos;
  Vec2f size;
  bool outline;
};

struct ImGuiTex2D {
  ImGuiTex2D* next;
  Tex2D* tex;
  Vec2f pos;
  Vec2f size;
};

struct ImGuiText {
  ImGuiText* next;
  Font* font;
  Vec4f color;
  Vec2f pos;
  cstr text;
};

struct ImGuiData {
  Vec2f size;
  Vec2f offset;
  float scroll;
};

struct ImGuiWidget {
  ImGuiWidget* prev;
  uint64 hash;
  uint32 index;
  Vec2f pos;
  Vec2f size;
};

struct ImGuiStyle {
  ImGuiStyle* prev;
  Font* font;
  Font* fontSubheading;
  Vec2f spacing;
  Vec2f padding;
  Vec2f scrollBarSize;

  Vec4f buttonColor;
  Vec4f buttonColorFocus;
  Vec4f frameColor;
  Vec4f textColor;
  Vec4f textColorFocus;
};

struct ImGuiLayer {
  ImGuiLayer* parent;
  ImGuiLayer* next;
  ImGuiLayer* children;

  Vec2f pos;
  Vec2f size;
  uint64 hash;
  uint32 index;
  bool clip;

  ImGuiTex2D* tex2DList;
  ImGuiRect* rectList;
  ImGuiPanel* panelList;
  ImGuiText* textList;
  ImGuiLine* lineList;
};

struct ImGuiLayout {
  ImGuiLayout* prev;
  Vec2f lower;
  Vec2f upper;
  Vec2f size;
  Vec2f spacing;
  int styleVars;
  bool horizontal;
};

struct ImGui {
  ImGuiLayer* layer;
  ImGuiLayer* layerLast;
  ImGuiLayout* layout;
  ImGuiWidget* widget;
  ImGuiWidget* widgetLast;
  ImGuiStyle* style;
  ImGuiClipRect* clipRect;
  ImGuiCursor* cursorStack;

  Vec2f cursor;
  Vec2f mouse;

  uint64 focus[FocusType_SIZE];
  uint64 dragging;

  bool activate;
  Vec2f forceSize;

  HashMap* data;

  /* TODO : Stack allocator. */
  MemPool* layoutPool;
  MemPool* widgetPool;
  MemPool* stylePool;
  MemPool* clipRectPool;
  MemPool* cursorPool;
  MemPool* tex2DPool;
  MemPool* panelPool;
  MemPool* rectPool;
  MemPool* textPool;
  MemPool* linePool;
} static self;

/* -------------------------------------------------------------------------- */

inline static void EmitLine (
  Vec4f const& color,
  Vec2f const& p1,
  Vec2f const& p2)
{
  ImGuiLine* e = (ImGuiLine*)MemPool_Alloc(self.linePool);
  e->color = color;
  e->p1 = p1;
  e->p2 = p2;
  e->next = self.layer->lineList;
  self.layer->lineList = e;
}

inline static void EmitPanel (
  Vec4f const& color,
  Vec2f const& pos,
  Vec2f const& size,
  float innerAlpha = 1.0f,
  float bevel = 4.0f)
{
  ImGuiPanel* e = (ImGuiPanel*)MemPool_Alloc(self.panelPool);
  e->color = color;
  e->pos = pos;
  e->size = size;
  e->innerAlpha = innerAlpha;
  e->bevel = bevel;
  e->next = self.layer->panelList;
  self.layer->panelList = e;
}

inline static void EmitRect (
  Vec4f const& color,
  Vec2f const& pos,
  Vec2f const& size,
  bool outline = false)
{
  ImGuiRect* e = (ImGuiRect*)MemPool_Alloc(self.rectPool);
  e->color = color;
  e->pos = pos;
  e->size = size;
  e->outline = outline;
  e->next = self.layer->rectList;
  self.layer->rectList = e;
}

inline static void EmitTex2D (
  Tex2D* tex,
  Vec2f const& pos,
  Vec2f const& size)
{
  ImGuiTex2D* e = (ImGuiTex2D*)MemPool_Alloc(self.tex2DPool);
  e->tex = tex;
  e->pos = pos;
  e->size = size;
  e->next = self.layer->tex2DList;
  self.layer->tex2DList = e;
}

inline static void EmitText (
  Font* font,
  Vec4f const& color,
  Vec2f const& pos,
  cstr text)
{
  ImGuiText* e = (ImGuiText*)MemPool_Alloc(self.textPool);
  e->font = font;
  e->color = color;
  e->pos = pos;
  e->text = StrDup(text);
  e->next = self.layer->textList;
  self.layer->textList = e;
}

/* -------------------------------------------------------------------------- */

inline static ImGuiData* GetData (uint64 hash) {
  ImGuiData* data = (ImGuiData*)HashMap_GetRaw(self.data, hash);
  if (!data) {
    data = MemNew(ImGuiData);
    data->size = Vec2f_Create(0, 0);
    data->offset = Vec2f_Create(0, 0);
    data->scroll = 0;
    HashMap_SetRaw(self.data, hash, data);
  }
  return data;
}

/* -------------------------------------------------------------------------- */

static void ImGui_PushDefaultStyle () {
  static Font* font = 0;
  static Font* fontSubheading = 0;
  if (!font) {
    font = Font_Load("Share", 16);
    fontSubheading = Font_Load("Iceland", 18);
  }

  ImGuiStyle* style = (ImGuiStyle*)MemPool_Alloc(self.stylePool);
  style->prev = self.style;
  style->font = font;
  style->fontSubheading = fontSubheading;
  style->spacing = Vec2f_Create(8.0f, 8.0f);
  style->padding = Vec2f_Create(8.0f, 8.0f);
  style->scrollBarSize = Vec2f_Create(4.0f, 4.0f);
  style->buttonColor = Vec4f_Create(0.1f, 0.12f, 0.15f, 1.0f);
  style->buttonColorFocus = Vec4f_Create(0.1f, 0.6f, 1.0f, 1.0f);
  style->frameColor = Vec4f_Create(0.1f, 0.12f, 0.15f, 0.95f);
  style->textColor = Vec4f_Create(1.0f, 1.0f, 1.0f, 1.0f);
  style->textColorFocus = Vec4f_Create(0.1f, 0.1f, 0.1f, 1.0f);
  self.style = style;
}

/* -------------------------------------------------------------------------- */

static void ImGui_PushClipRect (Vec2f pos, Vec2f size) {
  ImGuiClipRect* rect = MemNew(ImGuiClipRect);
  ImGuiClipRect* prev = self.clipRect;
  rect->prev = prev;
  rect->p1 = pos;
  rect->p2 = Vec2f_Add(pos, size);
  if (prev) {
    rect->p1.x = Max(rect->p1.x, prev->p1.x);
    rect->p1.y = Max(rect->p1.y, prev->p1.y);
    rect->p2.x = Min(rect->p2.x, prev->p2.x);
    rect->p2.y = Min(rect->p2.y, prev->p2.y);
  }
  self.clipRect = rect;
}

static void ImGui_PopClipRect () {
  ImGuiClipRect* rect = self.clipRect;
  self.clipRect = rect->prev;
  MemFree(rect);
}

inline static bool IsClipped (Vec2f p) {
  if (!self.clipRect) return false;
  return
    p.x < self.clipRect->p1.x ||
    p.y < self.clipRect->p1.y ||
    self.clipRect->p2.x < p.x ||
    self.clipRect->p2.y < p.y;
}

/* -------------------------------------------------------------------------- */

inline static void Advance (Vec2f size) {
  if (self.layout->horizontal) {
    self.cursor.x += size.x;
    self.layout->spacing.x = self.style->spacing.x;
  } else {
    self.cursor.y += size.y;
    self.layout->spacing.y = self.style->spacing.y;
  }
}

inline static uint64 HashGet () {
  return Hash_FNV64_Incremental(
     self.widget->hash,
    &self.widget->index,
    sizeof(self.widget->index));
}

inline static uint64 HashNext () {
  self.widget->index++;
  return HashGet();
}

inline static uint64 HashPeekNext () {
  uint32 index = self.widget->index + 1;
  return Hash_FNV64_Incremental(self.widget->hash, &index, sizeof(index));
}

inline static void TransformPos (float& x, float& y) {
  if (x < 0.0f) x = self.layout->upper.x + x;
  if (y < 0.0f) y = self.layout->upper.y + y;
}

inline static void TransformSize (float& sx, float& sy) {
  if (sx <= 0.0f) sx = (self.layout->upper.x - self.cursor.x) + sx;
  if (sy <= 0.0f) sy = (self.layout->upper.y - self.cursor.y) + sy;
}

inline static Vec2f GetNextSize (Vec2f s) {
  TransformSize(s.x, s.y);
  return s;
}

inline static bool RectContains (Vec2f pos, Vec2f size, Vec2f p) {
  return
    pos.x <= p.x && p.x <= pos.x + size.x &&
    pos.y <= p.y && p.y <= pos.y + size.y;
}

inline static void Spacing () {
  self.cursor.x += self.layout->spacing.x;
  self.cursor.y += self.layout->spacing.y;
  self.layout->spacing.x = 0;
  self.layout->spacing.y = 0;
}

static void ImGui_PushLayout (float sx, float sy, bool horizontal) {
  TransformSize(sx, sy);
  ImGuiLayout* layout = (ImGuiLayout*)MemPool_Alloc(self.layoutPool);
  layout->prev = self.layout;
  layout->lower = self.cursor;
  layout->upper = Vec2f_Create(self.cursor.x + sx, self.cursor.y + sy);
  layout->size = Vec2f_Create(sx, sy);
  layout->styleVars = 0;
  layout->horizontal = horizontal;
  self.layout = layout;
}

static void ImGui_PopLayout () {
  ImGuiLayout* layout = self.layout;
  for (int i = 0; i < layout->styleVars; ++i)
    ImGui_PopStyle();
  self.layout = layout->prev;
  MemPool_Dealloc(self.layoutPool, layout);
}

inline static void ImGui_Pad (float mx = 1.0f, float my = 1.0f) {
  float px = mx * self.style->padding.x;
  float py = my * self.style->padding.y;
  self.cursor.x += px;
  self.cursor.y += py;
  self.layout->lower.x += px;
  self.layout->lower.y += py;
  self.layout->upper.x -= px;
  self.layout->upper.y -= py;
  self.layout->size.x -= 2.0f * px;
  self.layout->size.y -= 2.0f * py;
}

static void ImGui_Unpad (float mx = 1.0f, float my = 1.0f) {
  float px = mx * self.style->padding.x;
  float py = my * self.style->padding.y;
  self.layout->lower.x -= px;
  self.layout->lower.y -= py;
  self.layout->upper.x += px;
  self.layout->upper.y += py;
  self.layout->size.x += 2.0f * px;
  self.layout->size.y += 2.0f * py;
  self.cursor.x -= px;
  self.cursor.y -= py;
}

/* -------------------------------------------------------------------------- */

static void ImGui_BeginWidget (float sx, float sy) {
  Spacing();
  TransformSize(sx, sy);
  ImGuiWidget* widget = (ImGuiWidget*)MemPool_Alloc(self.widgetPool);

  widget->prev = self.widget;
  widget->index = 0;
  widget->pos = Vec2f_Create(self.cursor.x, self.cursor.y);
  widget->size = Vec2f_Create(sx, sy);

  /* Compute this widget's hash. */ {
    if (self.widget) {
      self.widget->index++;
      widget->hash = Hash_FNV64_Incremental(
         self.widget->hash,
        &self.widget->index,
        sizeof(self.widget->index));
    } else {
      widget->hash = Hash_FNV64_Init();
    }
  }

  self.widget = widget;
}

static void ImGui_EndWidget () {
  if (self.widgetLast)
    MemPool_Dealloc(self.widgetPool, self.widgetLast);
  self.cursor = self.widget->pos;
  self.widgetLast = self.widget;
  self.widget = self.widget->prev;
  Advance(self.widgetLast->size);
}

static bool ImGui_Focus (ImGuiWidget* widget, int focusType) {
  if (self.focus[focusType] == 0)
    if (!IsClipped(self.mouse) && RectContains(widget->pos, widget->size, self.mouse))
      self.focus[focusType] = widget->hash;
  return self.focus[focusType] == widget->hash;
}

inline static bool ImGui_FocusCurrent (int focusType) {
  return ImGui_Focus(self.widget, focusType);
}

inline static bool ImGui_FocusLast (int focusType) {
  return ImGui_Focus(self.widgetLast, focusType);
}



inline bool HasFocus (uint64 hash, int focusType) {
  return self.focus[focusType] == hash;
}

inline bool TryFocusRect (uint64 hash, int focusType, Vec2f pos, Vec2f size) {
  if (self.focus[focusType] == 0)
    if (!IsClipped(self.mouse) && RectContains(pos, size, self.mouse))
      self.focus[focusType] = hash;
  return self.focus[focusType] == hash;
}

/* -------------------------------------------------------------------------- */

static void ImGuiLayer_Free (ImGuiLayer* self) {
  ImGuiLayer* child = self->children;
  while (child) {
    ImGuiLayer* next = child->next;
    ImGuiLayer_Free(child);
    child = next;
  }

  /* TODO : Stack allocation for strs. */
  for (ImGuiText* e = self->textList; e; e = e->next)
    StrFree(e->text);
  MemFree(self);
}

static ImGuiLayer* ImGui_PushLayer (bool clip) {
  ImGuiLayer* layer = MemNew(ImGuiLayer);
  layer->parent = self.layer;
  layer->children = 0;
  layer->next = 0;

  layer->pos = self.layout->lower;
  layer->size = self.layout->size;
  layer->index = 0;
  layer->clip = clip;

  layer->tex2DList = 0;
  layer->panelList = 0;
  layer->rectList = 0;
  layer->textList = 0;
  layer->lineList = 0;

  if (self.layer) {
    layer->next = self.layer->children;
    self.layer->children = layer;
    layer->hash = HashNext();
  } else {
    layer->hash = Hash_FNV64_Init();
  }

  self.layer = layer;
  if (clip)
    ImGui_PushClipRect(self.layer->pos, self.layer->size);
  return layer;
}

static void ImGui_PopLayer () {
  if (self.layer->clip)
    ImGui_PopClipRect();
  self.layerLast = self.layer;
  self.layer = self.layer->parent;
}

static void ImGui_DrawLayer (ImGuiLayer const* self) {
  if (self->clip)
    ClipRect_PushCombined(UNPACK2(self->pos), UNPACK2(self->size));

  for (ImGuiTex2D const* e = self->tex2DList; e; e = e->next) {
    Draw_Color(1, 1, 1, 1);
    Tex2D_Draw(e->tex, UNPACK2(e->pos), UNPACK2(e->size));
  }

  if (self->panelList) {
    static Shader* shader = 0;
    if (!shader)
      shader = Shader_Load("vertex/ui", "fragment/ui/panel");

    const float pad = 64.0f;
    Shader_Start(shader);
    Shader_SetFloat("padding", pad);

    for (ImGuiPanel const* e = self->panelList; e; e = e->next) {
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

  for (ImGuiRect const* e = self->rectList; e; e = e->next) {
    Draw_Color(UNPACK4(e->color));
    if (e->outline) {
      Draw_Border(1.0f, UNPACK2(e->pos), UNPACK2(e->size));
    } else {
      Draw_Rect(UNPACK2(e->pos), UNPACK2(e->size));
    }
  }

#if 0
  for (ImGuiLine const* e = self->lineList; e; e = e->next) {
    Draw_Color(UNPACK4(e->color));
    Draw_Line(UNPACK2(e->p1), UNPACK2(e->p2));
  }
#else
  if (self->lineList) {
    RenderState_PushBlendMode(BlendMode_Additive);
    static Shader* shader = 0;
    if (!shader)
      shader = Shader_Load("vertex/ui", "fragment/ui/line");
    const float pad = 64.0f;
    Shader_Start(shader);
    for (ImGuiLine const* e = self->lineList; e; e = e->next) {
      float xMin = Min(e->p1.x, e->p2.x) - pad;
      float yMin = Min(e->p1.y, e->p2.y) - pad;
      float xMax = Max(e->p1.x, e->p2.x) + pad;
      float yMax = Max(e->p1.y, e->p2.y) + pad;
      float sx = xMax - xMin;
      float sy = yMax - yMin;
      Shader_SetFloat2("origin", xMin, yMin);
      Shader_SetFloat2("size", sx, sy);
      Shader_SetFloat2("p1", e->p1.x, e->p1.y);
      Shader_SetFloat2("p2", e->p2.x, e->p2.y);
      Shader_SetFloat4("color", UNPACK4(e->color));
      Draw_Rect(xMin, yMin, sx, sy);
    }
    Shader_Stop(shader);
    RenderState_PopBlendMode();
  }
#endif

  for (ImGuiText const* e = self->textList; e; e = e->next)
    Font_Draw(e->font, e->text, UNPACK2(e->pos), UNPACK4(e->color));

  for (ImGuiLayer const* e = self->children; e; e = e->next)
    ImGui_DrawLayer(e);

  if (self->clip)
    ClipRect_Pop();
}

#include "BlendMode.h"
#include "Button.h"
#include "ClipRect.h"
#include "Draw.h"
#include "Font.h"
#include "Hash.h"
#include "HashMap.h"
#include "HmGui.h"
#include "Input.h"
#include "PhxMemory.h"
#include "PhxString.h"
#include "Profiler.h"
#include "RenderState.h"
#include "Tex2D.h"
#include "UIRenderer.h"

#define HMGUI_DRAW_GROUP_FRAMES 0
#define HMGUI_ENABLE_DRAGGING 1

const uint32 Layout_None = 0;
const uint32 Layout_Stack = 1;
const uint32 Layout_Vertical = 2;
const uint32 Layout_Horizontal = 3;

const uint32 Widget_Group = 0;
const uint32 Widget_Text = 1;
const uint32 Widget_Rect = 2;
const uint32 Widget_Image = 3;

const uint32 FocusStyle_None = 0;
const uint32 FocusStyle_Fill = 1;
const uint32 FocusStyle_Outline = 2;
const uint32 FocusStyle_Underline = 3;

const int FocusType_Mouse = 0;
const int FocusType_Scroll = 1;
const int FocusType_SIZE = 2;

struct HmGuiWidget {
  struct HmGuiGroup* parent;
  HmGuiWidget* next;
  HmGuiWidget* prev;
  uint64 hash;
  uint32 type;
  Vec2f pos;
  Vec2f size;
  Vec2f minSize;
  Vec2f align;
  Vec2f stretch;
};

struct HmGuiGroup : public HmGuiWidget {
  HmGuiWidget* head;
  HmGuiWidget* tail;
  uint32 layout;
  uint32 children;
  uint32 focusStyle;
  Vec2f paddingLower;
  Vec2f paddingUpper;
  Vec2f offset;
  Vec2f maxSize;
  Vec2f totalStretch;
  float spacing;
  float frameOpacity;
  bool clip;
  bool expand;
  bool focusable[FocusType_SIZE];
  bool storeSize;
};

struct HmGuiText : public HmGuiWidget {
  Font* font;
  cstr text;
  Vec4f color;
};

struct HmGuiRect : public HmGuiWidget {
  Vec4f color;
};

struct HmGuiImage : public HmGuiWidget {
  Tex2D* image;
};

struct HmGuiClipRect {
  HmGuiClipRect* prev;
  Vec2f lower;
  Vec2f upper;
};

struct HmGuiData {
  Vec2f offset;
  Vec2f minSize;
  Vec2f size;
};

struct HmGuiStyle {
  HmGuiStyle* prev;
  Font* font;
  float spacing;
  Vec4f colorPrimary;
  Vec4f colorFrame;
  Vec4f colorText;
};

struct HmGui {
  HmGuiGroup* group;
  HmGuiGroup* root;
  HmGuiWidget* last;
  HmGuiStyle* style;
  HmGuiClipRect* clipRect;
  HashMap* data;

  uint64 focus[FocusType_SIZE];
  Vec2f focusPos;
  bool activate;
} static self = { 0 };

static bool init = false;

#include "HmGuiInternal.h"

void HmGui_Begin (float sx, float sy) {
  if (!init) {
    init = true;
    self.group = 0;
    self.root = 0;

    self.style = MemNew(HmGuiStyle);
    self.style->prev = 0;
    self.style->font = Font_Load("Rajdhani", 14);
    self.style->spacing = 6;

    self.style->colorPrimary = Vec4f_Create(0.1f, 0.5f, 1.0f, 1.0f);
    self.style->colorFrame = Vec4f_Create(0.1f, 0.1f, 0.1f, 0.5f);
    self.style->colorText = Vec4f_Create(1, 1, 1, 1);

    self.clipRect = 0;
    self.data = HashMap_Create(0, 128);

    for (int i = 0; i < FocusType_SIZE; ++i) self.focus[i] = 0;
    self.activate = false;
  }

  if (self.root) {
    HmGui_FreeGroup(self.root);
    self.root = 0;
  }
  self.last = 0;
  self.activate = Input_GetPressed(Button_Mouse_Left);

  HmGui_BeginGroup(Layout_None);
  self.group->clip = true;
  self.group->pos = Vec2f_Create(0, 0);
  self.group->size = Vec2f_Create(sx, sy);
  self.root = self.group;
}

void HmGui_End () {
  FRAME_BEGIN;
  HmGui_EndGroup();
  HmGui_ComputeSize(self.root);
  HmGui_LayoutGroup(self.root);

  for (int i = 0; i < FocusType_SIZE; ++i) self.focus[i] = 0;
  Vec2i mouse; Input_GetMousePosition(&mouse);
  self.focusPos = Vec2f_Create(mouse.x, mouse.y);
  HmGui_CheckFocus(self.root);
  FRAME_END;
}

void HmGui_Draw () {
  FRAME_BEGIN;
  RenderState_PushBlendMode(BlendMode_Alpha);
  UIRenderer_Begin();
  HmGui_DrawGroup(self.root);
  UIRenderer_End();
  RenderState_PopBlendMode();
  UIRenderer_Draw();
  FRAME_END;
}

/* -------------------------------------------------------------------------- */

void HmGui_BeginGroupX () {
  HmGui_BeginGroup(Layout_Horizontal);
}

void HmGui_BeginGroupY () {
  HmGui_BeginGroup(Layout_Vertical);
}

void HmGui_BeginGroupStack () {
  HmGui_BeginGroup(Layout_Stack);
}

void HmGui_EndGroup () {
  self.last = self.group;
  self.group = self.group->parent;
}

void HmGui_BeginScroll (float maxSize) {
  HmGui_BeginGroupX();
  HmGui_SetStretch(1, 1);
  self.group->clip = true;
  HmGui_SetSpacing(2);
  HmGui_BeginGroupY();
  HmGui_SetPadding(6, 6);
  HmGui_SetStretch(1, 1);
  self.group->expand = false;
  self.group->storeSize = true;
  self.group->maxSize.y = maxSize;

  HmGuiData* data = HmGui_GetData(self.group);
  self.group->offset.y = -data->offset.y;
}

void HmGui_EndScroll () {
  HmGuiData* data = HmGui_GetData(self.group);
  if (HmGui_GroupHasFocus(FocusType_Scroll)) {
    Vec2i scroll; Input_GetMouseScroll(&scroll);
    data->offset.y -= 10.0f * scroll.y;
  }

  float maxScroll = Max(0.0f, data->minSize.y - data->size.y);
  data->offset.y = Clamp(data->offset.y, 0.0f, maxScroll);

  HmGui_EndGroup();

  HmGui_BeginGroupY();
  HmGui_SetStretch(0, 1);
  HmGui_SetSpacing(0);
  if (maxScroll > 0) {
    float handleSize = data->size.y * (data->size.y / data->minSize.y);
    float handlePos = Lerp(0.0f, data->size.y - handleSize, data->offset.y / maxScroll);
    HmGui_Rect(4.0f, handlePos, 0.0f, 0.0f, 0.0f, 0.0f);
    HmGui_Rect(4.0f, handleSize, UNPACK4(self.style->colorFrame));
  } else {
    HmGui_Rect(4.0f, 16.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  }
  HmGui_EndGroup();
  HmGui_EndGroup();
}

void HmGui_BeginWindow (cstr title) {
  HmGui_BeginGroupStack();
  HmGui_SetStretch(0, 0);
  self.group->focusStyle = FocusStyle_None;
  self.group->frameOpacity = 0.95f;
#if HMGUI_ENABLE_DRAGGING
  HmGuiData* data = HmGui_GetData(self.group);
  if (HmGui_GroupHasFocus(FocusType_Mouse)) {
    if (Input_GetDown(Button_Mouse_Left)) {
      Vec2i md; Input_GetMouseDelta(&md);
      data->offset.x += md.x;
      data->offset.y += md.y;
    }
  }

  self.group->pos.x += data->offset.x;
  self.group->pos.y += data->offset.y;
#endif

  HmGui_BeginGroupY();
  self.group->clip = true;
  HmGui_SetPadding(8, 8);
  HmGui_SetStretch(1, 1);
  // HmGui_TextColored(title, 1.0f, 1.0f, 1.0f, 0.3f);
  // HmGui_SetAlign(0.5f, 0.0f);
}

void HmGui_EndWindow () {
  HmGui_EndGroup();
  HmGui_EndGroup();
}

/* -------------------------------------------------------------------------- */

bool HmGui_Button (cstr label) {
  HmGui_BeginGroupStack();
  self.group->focusStyle = FocusStyle_Fill;
  self.group->frameOpacity = 0.5f;
  bool focus = HmGui_GroupHasFocus(FocusType_Mouse);
  HmGui_SetPadding(8, 8);
  HmGui_Text(label);
  HmGui_SetAlign(0.5f, 0.5f);
  HmGui_EndGroup();
  return focus && self.activate;
}

bool HmGui_Checkbox (cstr label, bool value) {
  HmGui_BeginGroupX();
  self.group->focusStyle = FocusStyle_Underline;
  if (HmGui_GroupHasFocus(FocusType_Mouse) && self.activate)
    value = !value;
  HmGui_SetPadding(4, 4);
  HmGui_SetSpacing(8);
  HmGui_SetStretch(1, 0);

  HmGui_Text(label);
  HmGui_SetAlign(0.0f, 0.5f);
  HmGui_SetStretch(1, 0);

  HmGui_BeginGroupStack();
  HmGui_Rect(16, 16, UNPACK4(self.style->colorFrame));
  if (value) {
    HmGui_Rect(10, 10, UNPACK4(self.style->colorPrimary));
    HmGui_SetAlign(0.5f, 0.5f);
  }
  HmGui_EndGroup();
  HmGui_SetStretch(0, 0);
  HmGui_EndGroup();
  return value;
}

float HmGui_Slider (float lower, float upper, float value) {
  HmGui_BeginGroupStack();
  HmGui_Rect(0, 2, 0.5f, 0.5f, 0.5f, 1.0f);
  HmGui_SetAlign(0.5f, 0.5f);
  HmGui_SetStretch(1, 0);
  HmGui_EndGroup();
  return 0.0f;
}

/* -------------------------------------------------------------------------- */

void HmGui_Image (Tex2D* image) {
  HmGuiImage* e = MemNew(HmGuiImage);
  HmGui_InitWidget(e, Widget_Image);
  e->image = image;
  e->stretch = Vec2f_Create(1, 1);
}

void HmGui_Rect (float sx, float sy, float r, float g, float b, float a) {
  HmGuiRect* e = MemNew(HmGuiRect);
  HmGui_InitWidget(e, Widget_Rect);
  e->color = Vec4f_Create(r, g, b, a);
  e->minSize = Vec2f_Create(sx, sy);
}

void HmGui_Text (cstr text) {
  HmGui_TextEx(self.style->font, text, UNPACK4(self.style->colorText));
}

void HmGui_TextColored (cstr text, float r, float g, float b, float a) {
  HmGui_TextEx(self.style->font, text, r, g, b, a);
}

void HmGui_TextEx (Font* font, cstr text, float r, float g, float b, float a) {
  HmGuiText* e = MemNew(HmGuiText);
  HmGui_InitWidget(e, Widget_Text);
  e->font = font;
  e->text = StrDup(text);
  e->color = Vec4f_Create(r, g, b, a);
  Vec2i size; Font_GetSize2(e->font, &size, e->text);
  e->minSize = Vec2f_Create(size.x, size.y);
  HmGui_SetAlign(0.0f, 1.0f);
}

/* -------------------------------------------------------------------------- */

void HmGui_SetAlign (float ax, float ay) {
  self.last->align = Vec2f_Create(ax, ay);
}

void HmGui_SetPadding (float px, float py) {
  self.group->paddingLower = Vec2f_Create(px, py);
  self.group->paddingUpper = Vec2f_Create(px, py);
}

void HmGui_SetPaddingEx (float left, float top, float right, float bottom) {
  self.group->paddingLower = Vec2f_Create(left, top);
  self.group->paddingUpper = Vec2f_Create(right, bottom);
}

void HmGui_SetPaddingLeft (float padding) {
  self.group->paddingLower.x = padding;
}

void HmGui_SetPaddingTop (float padding) {
  self.group->paddingLower.y = padding;
}

void HmGui_SetPaddingRight (float padding) {
  self.group->paddingUpper.x = padding;
}

void HmGui_SetPaddingBottom (float padding) {
  self.group->paddingUpper.y = padding;
}

void HmGui_SetSpacing (float spacing) {
  self.group->spacing = spacing;
}

void HmGui_SetStretch (float x, float y) {
  self.last->stretch = Vec2f_Create(x, y);
}

bool HmGui_GroupHasFocus (int type) {
  self.group->focusable[type] = true;
  return self.focus[type] == self.group->hash;
}

/* -------------------------------------------------------------------------- */

void HmGui_PushStyle () {
  HmGuiStyle* style = MemNew(HmGuiStyle);
  *style = *self.style;
  style->prev = self.style;
  self.style = style;
}

void HmGui_PushFont (Font* font) {
  HmGui_PushStyle();
  self.style->font = font;
}

void HmGui_PushTextColor (float r, float g, float b, float a) {
  HmGui_PushStyle();
  self.style->colorText = Vec4f_Create(r, g, b, a);
}

void HmGui_PopStyle (int depth) {
  for (int i = 0; i < depth; ++i) {
    HmGuiStyle* style = self.style;
    self.style = style->prev;
    MemFree(style);
  }
}

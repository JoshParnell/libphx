#include "ArrayList.h"
#include "BlendMode.h"
#include "Button.h"
#include "ClipRect.h"
#include "Draw.h"
#include "Font.h"
#include "Hash.h"
#include "HashMap.h"
#include "ImGui.h"
#include "Input.h"
#include "MemPool.h"
#include "PhxString.h"
#include "RenderState.h"
#include "Shader.h"
#include "Tex2D.h"
#include "Vec2.h"
#include "Vec4.h"
#include "Viewport.h"

#include <stdio.h>

#include "ImGuiInternal.h"

static bool init = false;

static void ImGui_Init () {
  if (init) return;
  init = true;
  self.layer = 0;
  self.layerLast = 0;
  self.style = 0;
  self.clipRect = 0;
  self.cursorStack = 0;
  self.dragging = 0;

  self.data = HashMap_Create(0, 128);
  self.layoutPool = MemPool_CreateAuto(sizeof(ImGuiLayout));
  self.widgetPool = MemPool_CreateAuto(sizeof(ImGuiWidget));
  self.stylePool = MemPool_CreateAuto(sizeof(ImGuiStyle));
  self.clipRectPool = MemPool_CreateAuto(sizeof(ImGuiClipRect));
  self.cursorPool = MemPool_CreateAuto(sizeof(ImGuiCursor));
  self.tex2DPool = MemPool_CreateAuto(sizeof(ImGuiTex2D));
  self.panelPool = MemPool_CreateAuto(sizeof(ImGuiPanel));
  self.rectPool = MemPool_CreateAuto(sizeof(ImGuiRect));
  self.textPool = MemPool_CreateAuto(sizeof(ImGuiText));
  self.linePool = MemPool_CreateAuto(sizeof(ImGuiLine));
}

/* -------------------------------------------------------------------------- */

void ImGui_Begin (float sx, float sy) {
  ImGui_Init();

  for (int i = 0; i < FocusType_SIZE; ++i)
    self.focus[i] = 0;

  if (!Input_GetDown(Button_Mouse_Left))
    self.dragging = 0;
  if (self.dragging)
    self.focus[FocusType_Mouse] = self.dragging;

  self.cursor = Vec2f_Create(0, 0);

  if (self.layerLast) {
    ImGuiLayer_Free(self.layerLast);
    self.layerLast = 0;
  }

  MemPool_Clear(self.layoutPool);
  MemPool_Clear(self.widgetPool);
  MemPool_Clear(self.stylePool);
  MemPool_Clear(self.clipRectPool);
  MemPool_Clear(self.cursorPool);
  MemPool_Clear(self.tex2DPool);
  MemPool_Clear(self.panelPool);
  MemPool_Clear(self.rectPool);
  MemPool_Clear(self.textPool);
  MemPool_Clear(self.linePool);

  self.style = 0;
  ImGui_PushDefaultStyle();
  self.layout = 0;
  ImGui_PushLayout(sx, sy, false);

  self.widget = 0;
  self.widgetLast = 0;
  ImGui_BeginWidget(sx, sy);

  self.layer = 0;
  ImGui_PushLayer(true);

  Vec2i mouse;
  Input_GetMousePosition(&mouse);
  self.mouse.x = (float)mouse.x;
  self.mouse.y = (float)mouse.y;

  self.activate = Input_GetPressed(Button_Mouse_Left);
  self.forceSize = Vec2f_Create(0, 0);
}

void ImGui_End () {
  ImGui_PopLayer();
  ImGui_EndWidget();
  ImGui_PopLayout();

  if (self.layer != 0) Fatal("ImGui_End: layer stack not empty");
  if (self.widget != 0) Fatal("ImGui_End: widget stack not empty");
  if (self.layout != 0) Fatal("ImGui_End: layout stack not empty");
}

void ImGui_Draw () {
  RenderState_PushBlendMode(BlendMode_Alpha);
  Draw_LineWidth(1);
  ImGui_DrawLayer(self.layerLast);
  RenderState_PopBlendMode();
}

/* -------------------------------------------------------------------------- */

void ImGui_AlignCursor (float sx, float sy, float alignX, float alignY) {
  TransformSize(sx, sy);
  ImGui_SetCursor(
    self.layout->lower.x + alignX * (self.layout->size.x - sx),
    self.layout->lower.y + alignY * (self.layout->size.y - sy));
}

float ImGui_GetCursorX () {
  return self.cursor.x;
}

float ImGui_GetCursorY () {
  return self.cursor.y;
}

void ImGui_PushCursor () {
  ImGuiCursor* cursor = (ImGuiCursor*)MemPool_Alloc(self.cursorPool);
  cursor->prev = self.cursorStack;
  cursor->pos = self.cursor;
  self.cursorStack = cursor;
}

void ImGui_PopCursor () {
  ImGuiCursor* cursor = self.cursorStack;
  self.cursor = cursor->pos;
  self.cursorStack = cursor->prev;
  MemPool_Dealloc(self.cursorPool, cursor);
}

void ImGui_SetCursor (float cx, float cy) {
  TransformPos(cx, cy);
  self.cursor = Vec2f_Create(cx, cy);
  self.layout->spacing = Vec2f_Create(0, 0);
}

void ImGui_SetCursorX (float x) {
  ImGui_SetCursor(x, self.cursor.y);
}

void ImGui_SetCursorY (float y) {
  ImGui_SetCursor(self.cursor.x, y);
}

void ImGui_Indent () {
  self.cursor.x += 2.0f * self.style->padding.x;
}

void ImGui_Undent () {
  self.cursor.x -= 2.0f * self.style->padding.x;
}

/* -------------------------------------------------------------------------- */

void ImGui_BeginGroup (float sx, float sy, bool horizontal) {
  ImGui_BeginWidget(sx, sy);
  ImGui_PushLayout(sx, sy, horizontal);
}

void ImGui_BeginGroupX (float sy) {
  ImGui_BeginWidget(0, sy);
  ImGui_PushLayout(0, sy, true);
}

void ImGui_BeginGroupY (float sx) {
  ImGui_BeginWidget(sx, 0);
  ImGui_PushLayout(sx, 0, false);
}

void ImGui_EndGroup () {
  ImGui_PopLayout();
  ImGui_EndWidget();
}

void ImGui_BeginPanel (float sx, float sy) {
  ImGui_BeginGroup(sx, sy, false);
  ImGui_PushLayer(false);
  ImGui_PushLayer(true);
  ImGui_Pad();
}

void ImGui_EndPanel () {
  ImGui_Unpad();
  ImGui_PopLayer();
  EmitPanel(self.style->frameColor, self.widget->pos, self.widget->size, 1.0f, 12.0f);
  ImGui_PopLayer();
  ImGui_EndGroup();
}

void ImGui_BeginWindow (cstr title, float sx, float sy) {
  uint64 hash = HashPeekNext();
  ImGuiData* data = GetData(hash);
  self.cursor.x += data->offset.x;
  self.cursor.y += data->offset.y;
  ImGui_BeginPanel(sx, sy);
  // ImGui_TextEx(self.style->fontSubheading, title, 0.4f, 0.4f, 0.4f, 1.0f);
}

void ImGui_EndWindow () {
  ImGui_EndPanel();
  ImGuiData* data = GetData(self.widgetLast->hash);
  self.cursor.x -= data->offset.x;
  self.cursor.y -= data->offset.y;

  if (ImGui_FocusLast(FocusType_Mouse)) {
    if (Input_GetDown(Button_Mouse_Left)) {
      Vec2i delta; Input_GetMouseDelta(&delta);
      data->offset.x += delta.x;
      data->offset.y += delta.y;
      self.dragging = self.widgetLast->hash;
    }
  }
}

void ImGui_BeginScrollFrame (float sx, float sy) {
  ImGui_BeginGroup(sx, sy, false);
  ImGui_PushLayer(true);
  ImGui_Pad();

  ImGuiData* data = GetData(self.widget->hash);
  self.cursor.y -= data->scroll;
}

void ImGui_EndScrollFrame () {
  ImGuiData* data = GetData(self.widget->hash);
  self.cursor.y += data->scroll;

  ImGuiLayout* layout = self.layout;
  ImGui_PopLayer();

  float scroll = data->scroll;
  float virtualSize = self.cursor.y - layout->lower.y;
  float scrollMax = virtualSize - layout->size.y;
  Vec2f scrollPos = Vec2f_Create(layout->lower.x + layout->size.x, layout->lower.y);
  Vec2f scrollSize = Vec2f_Create(self.style->scrollBarSize.x, layout->size.y);

  uint64 handleHash = HashNext();

  if (layout->size.y < virtualSize) {
    float handleSizeY = layout->size.y * (layout->size.y / virtualSize);
    handleSizeY = Clamp(handleSizeY, 16.0f, 128.0f);
    float handleOffset = (layout->size.y - handleSizeY) * (scroll / scrollMax);

    Vec2f handlePos = Vec2f_Create(scrollPos.x, scrollPos.y + handleOffset);
    Vec2f handleSize = Vec2f_Create(self.style->scrollBarSize.x, handleSizeY);
    bool handleFocus = TryFocusRect(handleHash, FocusType_Mouse, handlePos, handleSize);

    EmitPanel(
      handleFocus ? self.style->buttonColorFocus : Vec4f_Create(0.3f, 0.4f, 0.5f, 1.0f),
      handlePos,
      handleSize,
      handleFocus ? 0.5f : 0.25f,
      4.0f);
  }

  ImGui_Unpad();
  ImGui_EndGroup();
  EmitPanel(
    Vec4f_Create(0.0f, 0.0f, 0.0f, 0.5f),
    self.widgetLast->pos,
    self.widgetLast->size,
    0.25f);

  if (ImGui_FocusLast(FocusType_Scroll)) {
    Vec2i scroll; Input_GetMouseScroll(&scroll);
    data->scroll -= 10.0f * scroll.y;
  }

  data->scroll = Clamp(data->scroll, 0.0f, scrollMax);
}

/* -------------------------------------------------------------------------- */

void ImGui_SetNextWidth (float sx) {
  self.forceSize.x = sx;
}

void ImGui_SetNextHeight (float sy) {
  self.forceSize.y = sy;
}

void ImGui_PushStyle () {
  ImGuiStyle* style = (ImGuiStyle*)MemPool_Alloc(self.stylePool);
  MemCpy(style, self.style, sizeof(ImGuiStyle));
  style->prev = self.style;
  self.style = style;
}

void ImGui_PushStyleFont (Font* font) {
  ImGui_PushStyle();
  self.style->font = font;
}

void ImGui_PushStylePadding (float px, float py) {
  ImGui_PushStyle();
  self.style->padding = Vec2f_Create(px, py);
}

void ImGui_PushStyleSpacing (float x, float y) {
  ImGui_PushStyle();
  self.style->spacing = Vec2f_Create(x, y);
}

void ImGui_PushStyleTextColor (float r, float g, float b, float a) {
  ImGui_PushStyle();
  self.style->textColor = Vec4f_Create(r, g, b, a);
}

void ImGui_PopStyle () {
  if (!self.style->prev)
    Fatal("ImGui_PopStyle: Attempting to pop an empty stack");
  ImGuiStyle* style = self.style;
  self.style = style->prev;
  MemPool_Dealloc(self.stylePool, style);
}

void ImGui_SetFont (Font* font) {
  ImGui_PushStyleFont(font);
  self.layout->styleVars++;
}

void ImGui_SetSpacing (float sx, float sy) {
  ImGui_PushStyleSpacing(sx, sy);
  self.layout->styleVars++;
}

/* -------------------------------------------------------------------------- */

bool ImGui_Button (cstr label) {
  return ImGui_ButtonEx(label, 0, 32);
}

bool ImGui_ButtonEx (cstr label, float sx, float sy) {
  ImGui_BeginWidget(sx, sy);
  bool focus = ImGui_FocusCurrent(FocusType_Mouse);
  Vec4f color = focus ? self.style->buttonColorFocus : self.style->buttonColor;
  EmitPanel(color, self.widget->pos, self.widget->size, focus ? 1.0f : 0.5f);

  Vec2i bound;
  Font_GetSize2(self.style->font, &bound, label);
  Vec2f labelPos = Vec2f_Create(
    self.widget->pos.x + 0.5f * (self.widget->size.x - bound.x),
    self.widget->pos.y + 0.5f * (self.widget->size.y - bound.y));
  Vec2f labelSize = Vec2f_Create(bound.x, bound.y);

  EmitText(
    self.style->font,
    focus ? self.style->textColorFocus : self.style->textColor,
    Vec2f_Create(labelPos.x, labelPos.y + bound.y),
    label);

  ImGui_EndWidget();
  return focus && self.activate;
}

bool ImGui_Checkbox (bool value) {
  ImGui_BeginWidget(16.0f, 16.0f);
  bool focus = ImGui_FocusCurrent(FocusType_Mouse);
  if (focus && self.activate)
    value = !value;
  if (focus)
    EmitRect(
      self.style->buttonColorFocus,
      self.widget->pos,
      self.widget->size,
      true);

  EmitPanel(
    value ? self.style->buttonColorFocus : self.style->buttonColor,
    Vec2f_Create(self.widget->pos.x + 2, self.widget->pos.y + 2),
    Vec2f_Create(self.widget->size.x - 4, self.widget->size.y - 4));

  ImGui_EndWidget();
  return value;
}

void ImGui_Divider () {
  ImGui_BeginWidget(
    self.layout->horizontal ? 2 : 0,
    self.layout->horizontal ? 0 : 2);
  EmitLine(
    self.style->buttonColorFocus,
    self.widget->pos,
    Vec2f_Create(
      self.widget->pos.x + (self.layout->horizontal ? 0 : self.widget->size.x),
      self.widget->pos.y + (self.layout->horizontal ? self.widget->size.y : 0)));
  ImGui_EndWidget();
}

bool ImGui_Selectable (cstr label) {
  Vec2i bound;
  Font_GetSize2(self.style->font, &bound, label);
  ImGui_BeginWidget(
    self.layout->horizontal ? bound.x + 4.0f : 0,
    self.layout->horizontal ? 0 : 4.0f + Font_GetLineHeight(self.style->font));

  bool focus = ImGui_FocusCurrent(FocusType_Mouse);
  if (focus)
    EmitRect(
      self.style->buttonColorFocus,
      self.widget->pos,
      self.widget->size);

  EmitText(
    self.style->font,
    focus ? self.style->textColorFocus : self.style->textColor,
    Vec2f_Create(
      self.widget->pos.x + 2.0f,
      self.widget->pos.y + bound.y + 0.5f * (self.widget->size.y - bound.y)),
    label);

  ImGui_EndWidget();
  return focus && self.activate;
}

void ImGui_Tex2D (Tex2D* tex) {
  Vec2i size; Tex2D_GetSize(tex, &size);
  Vec2f sizef = Vec2f_Create(size.x, size.y);
  ImGui_BeginWidget(size.x, size.y);
  EmitTex2D(tex, self.cursor, sizef);
  ImGui_EndWidget();
}

void ImGui_Text (cstr text) {
  ImGui_TextEx(self.style->font, text, UNPACK4(self.style->textColor));
}

void ImGui_TextColored (cstr text, float r, float g, float b, float a) {
  ImGui_TextEx(self.style->font, text, r, g, b, a);
}

void ImGui_TextEx (Font* font, cstr text, float r, float g, float b, float a) {
  Vec2i bound;
  Font_GetSize2(self.style->font, &bound, text);
  ImGui_BeginWidget(bound.x, self.layout->horizontal ? 0 : Font_GetLineHeight(self.style->font));
  EmitText(
    font,
    Vec4f_Create(r, g, b, a),
    Vec2f_Create(
      self.widget->pos.x,
      self.widget->pos.y + bound.y + 0.5f * (self.widget->size.y - bound.y)),
    text);
  ImGui_EndWidget();
}

#ifndef PHX_UIRenderer
#define PHX_UIRenderer

#include "Common.h"

PHX_API void UIRenderer_Begin ();
PHX_API void UIRenderer_End   ();
PHX_API void UIRenderer_Draw  ();

PHX_API void UIRenderer_BeginLayer (float x, float y, float sx, float sy, bool clip);
PHX_API void UIRenderer_EndLayer   ();

PHX_API void UIRenderer_Image (Tex2D*, float x, float y, float sx, float sy);

PHX_API void UIRenderer_Panel (float x, float y, float sx, float sy,
                               float r, float g, float b, float a,
                               float bevel, float innerAlpha);

PHX_API void UIRenderer_Rect  (float x, float y, float sx, float sy,
                               float r, float g, float b, float a,
                               bool outline);

PHX_API void UIRenderer_Text  (Font* font, cstr text, float x, float y,
                               float r, float g, float b, float a);

#endif

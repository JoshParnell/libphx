#ifndef PHX_HmGui
#define PHX_HmGui

#include "Common.h"

PHX_API void  HmGui_Begin (float sx, float sy);
PHX_API void  HmGui_End   ();
PHX_API void  HmGui_Draw  ();

PHX_API void  HmGui_BeginGroupX      ();
PHX_API void  HmGui_BeginGroupY      ();
PHX_API void  HmGui_BeginGroupStack  ();
PHX_API void  HmGui_EndGroup         ();

PHX_API void  HmGui_BeginScroll (float maxSize);
PHX_API void  HmGui_EndScroll   ();

PHX_API void  HmGui_BeginWindow (cstr title);
PHX_API void  HmGui_EndWindow   ();

PHX_API bool  HmGui_Button   (cstr);
PHX_API bool  HmGui_Checkbox (cstr label, bool value);
PHX_API float HmGui_Slider   (float lower, float upper, float value);

PHX_API void  HmGui_Rect        (float sx, float sy, float r, float g, float b, float a);
PHX_API void  HmGui_Image       (Tex2D*);
PHX_API void  HmGui_Text        (cstr text);
PHX_API void  HmGui_TextColored (cstr text, float r, float g, float b, float a);
PHX_API void  HmGui_TextEx      (Font* font, cstr text, float r, float g, float b, float a);

PHX_API void  HmGui_SetAlign         (float ax, float ay);
PHX_API void  HmGui_SetPadding       (float px, float py);
PHX_API void  HmGui_SetPaddingEx     (float left, float top, float right, float bottom);
PHX_API void  HmGui_SetPaddingLeft   (float);
PHX_API void  HmGui_SetPaddingTop    (float);
PHX_API void  HmGui_SetPaddingRight  (float);
PHX_API void  HmGui_SetPaddingBottom (float);
PHX_API void  HmGui_SetSpacing       (float);
PHX_API void  HmGui_SetStretch       (float x, float y);

PHX_API bool  HmGui_GroupHasFocus (int type);

PHX_API void  HmGui_PushStyle     ();
PHX_API void  HmGui_PushFont      (Font*);
PHX_API void  HmGui_PushTextColor (float r, float g, float b, float a);
PHX_API void  HmGui_PopStyle      (int depth);

#endif

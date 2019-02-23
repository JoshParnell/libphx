#ifndef PHX_ImGui
#define PHX_ImGui

#include "Common.h"

PHX_API void ImGui_Begin (float sx, float sy);
PHX_API void ImGui_End   ();
PHX_API void ImGui_Draw  ();

PHX_API void  ImGui_AlignCursor (float sx, float sy, float alignX, float alignY);
PHX_API float ImGui_GetCursorX  ();
PHX_API float ImGui_GetCursorY  ();
PHX_API void  ImGui_PushCursor  ();
PHX_API void  ImGui_PopCursor   ();
PHX_API void  ImGui_SetCursor   (float x, float y);
PHX_API void  ImGui_SetCursorX  (float x);
PHX_API void  ImGui_SetCursorY  (float y);
PHX_API void  ImGui_Indent      ();
PHX_API void  ImGui_Undent      ();

PHX_API bool ImGui_Button      (cstr label);
PHX_API bool ImGui_ButtonEx    (cstr label, float sx, float sy);
PHX_API bool ImGui_Checkbox    (bool value);
PHX_API void ImGui_Divider     ();
PHX_API bool ImGui_Selectable  (cstr label);
PHX_API void ImGui_Tex2D       (Tex2D*);
PHX_API void ImGui_Text        (cstr text);
PHX_API void ImGui_TextColored (cstr text, float r, float g, float b, float a);
PHX_API void ImGui_TextEx      (Font* font, cstr text, float r, float g, float b, float a);

PHX_API void ImGui_BeginGroupX (float sy);
PHX_API void ImGui_BeginGroupY (float sx);
PHX_API void ImGui_EndGroup    ();

PHX_API void ImGui_BeginPanel  (float sx, float sy);
PHX_API void ImGui_EndPanel    ();

PHX_API void ImGui_BeginWindow (cstr title, float sx, float sy);
PHX_API void ImGui_EndWindow   ();

PHX_API void ImGui_BeginScrollFrame (float sx, float sy);
PHX_API void ImGui_EndScrollFrame   ();

PHX_API void ImGui_PushStyle          ();
PHX_API void ImGui_PushStyleFont      (Font*);
PHX_API void ImGui_PushStylePadding   (float px, float py);
PHX_API void ImGui_PushStyleSpacing   (float sx, float sy);
PHX_API void ImGui_PushStyleTextColor (float r, float g, float b, float a);
PHX_API void ImGui_PopStyle           ();

PHX_API void ImGui_SetFont            (Font*);
PHX_API void ImGui_SetSpacing         (float sx, float sy);

PHX_API void ImGui_SetNextWidth  (float sx);
PHX_API void ImGui_SetNextHeight (float sy);

#endif

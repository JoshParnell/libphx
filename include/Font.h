#ifndef PHX_Font
#define PHX_Font

#include "Common.h"
#include "Vec4.h"

/* --- Font --------------------------------------------------------------------
 *
 *   This type is REFERENCE-COUNTED. See ../doc/RefCounted.txt for details.
 *
 *   Font_DrawShaded : In addition to drawing glyphs as textured quads,
 *                     DrawShaded binds the texture for the current glyph to
 *                     the uniform sampler2D "glyph" of the currently-active
 *                     shader. This allows for text to be rendered through
 *                     the shader pipeline and, for example, output HDR values
 *                     for glyph colors.
 *
 * -------------------------------------------------------------------------- */

PHX_API Font*  Font_Load          (cstr name, int size);
PHX_API void   Font_Acquire       (Font*);
PHX_API void   Font_Free          (Font*);

PHX_API void   Font_Draw          (Font*, cstr text, float x, float y,
                                   float r, float g, float b, float a);
PHX_API void   Font_DrawShaded    (Font*, cstr text, float x, float y);

PHX_API int    Font_GetLineHeight (Font*);

PHX_API void   Font_GetSize       (Font*, Vec4i* out, cstr text);
PHX_API void   Font_GetSize2      (Font*, Vec2i* out, cstr text);

#endif

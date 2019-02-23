#ifndef PHX_ClipRect
#define PHX_ClipRect

#include "Common.h"

/* --- ClipRect ----------------------------------------------------------------
 *
 *   Uses OpenGL's scissoring functionality to prevent drawing outside a
 *   certain rectangular region of the current viewport.
 *
 *     ClipRect_[Push/Pop]Transform : Specify a transform for the ClipRect stack.
 *                                    Used to allow rects to remain relative to
 *                                    a local viewport or base resolution.
 *
 * -------------------------------------------------------------------------- */

PHX_API void  ClipRect_Push           (float x, float y, float sx, float sy);
PHX_API void  ClipRect_PushCombined   (float x, float y, float sx, float sy);
PHX_API void  ClipRect_PushDisabled   ();
PHX_API void  ClipRect_PushTransform  (float tx, float ty, float sx, float sy);
PHX_API void  ClipRect_Pop            ();
PHX_API void  ClipRect_PopTransform   ();

#endif

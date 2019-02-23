#ifndef PHX_Draw
#define PHX_Draw

#include "Common.h"
#include "Box3.h"
#include "Vec2.h"

/* TODO : This API is a bit hapazard with respect to using floats or vectors. */

/* --- Shared API ----------------------------------------------------------- */

PHX_API void  Draw_Clear         (float r, float g, float b, float a);
PHX_API void  Draw_ClearDepth    (float d);
PHX_API void  Draw_Color         (float r, float g, float b, float a);
PHX_API void  Draw_Flush         ();
PHX_API void  Draw_LineWidth     (float width);
PHX_API void  Draw_PointSize     (float size);
PHX_API void  Draw_PushAlpha     (float a);
PHX_API void  Draw_PopAlpha      ();
PHX_API void  Draw_SmoothLines   (bool);
PHX_API void  Draw_SmoothPoints  (bool);

/* --- 2D API --------------------------------------------------------------- */

PHX_API void  Draw_Border        (float s, float x, float y, float w, float h);
PHX_API void  Draw_Line          (float x1, float y1, float x2, float y2);
PHX_API void  Draw_Point         (float x, float y);
PHX_API void  Draw_Poly          (Vec2f const* points, int count);
PHX_API void  Draw_Quad          (Vec2f const* p1, Vec2f const* p2, Vec2f const* p3, Vec2f const* p4);
PHX_API void  Draw_Rect          (float x, float y, float sx, float sy);
PHX_API void  Draw_Tri           (Vec2f const* p1, Vec2f const* p2, Vec2f const* p3);

/* --- 3D API --------------------------------------------------------------- */

PHX_API void  Draw_Axes          (Vec3f const* pos,
                                  Vec3f const* x,
                                  Vec3f const* y,
                                  Vec3f const* z,
                                  float scale,
                                  float alpha);
PHX_API void  Draw_Box3          (Box3f const* box);
PHX_API void  Draw_Line3         (Vec3f const* p1, Vec3f const* p2);
PHX_API void  Draw_Plane         (Vec3f const* p, Vec3f const* n, float scale);
PHX_API void  Draw_Point3        (float x, float y, float z);
PHX_API void  Draw_Poly3         (Vec3f const* points, int count);
PHX_API void  Draw_Quad3         (Vec3f const* p1, Vec3f const* p2, Vec3f const* p3, Vec3f const* p4);
PHX_API void  Draw_Sphere        (Vec3f const* p, float r);
PHX_API void  Draw_Tri3          (Vec3f const* p1, Vec3f const* p2, Vec3f const* p3);

#endif

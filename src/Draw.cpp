#include "Draw.h"
#include "Metric.h"
#include "OpenGL.h"
#include "Vec4.h"

/* TODO JP : Replace all immediates with static VBO/IBOs & glDraw*. */

#define MAX_STACK_DEPTH 16

static float alphaStack[MAX_STACK_DEPTH];
static int alphaIndex = -1;
static Vec4f color = { 1, 1, 1, 1 };


void Draw_PushAlpha (float a) {
  if (alphaIndex + 1 >= MAX_STACK_DEPTH)
      Fatal("Draw_PushAlpha: Maximum alpha stack depth exceeded");

  float prevAlpha = alphaIndex >= 0 ? alphaStack[alphaIndex] : 1;
  float alpha = a * prevAlpha;
  alphaStack[++alphaIndex] = alpha;
  GLCALL(glColor4f(color.x, color.y, color.z, color.w * alpha));
}

void Draw_PopAlpha () {
  if (alphaIndex < 0)
      Fatal("Draw_PopAlpha Attempting to pop an empty alpha stack");

  alphaIndex--;
  float alpha = alphaIndex >= 0 ? alphaStack[alphaIndex] : 1;
  GLCALL(glColor4f(color.x, color.y, color.z, color.w * alpha));
}

void Draw_Axes (
  Vec3f const* pos,
  Vec3f const* x,
  Vec3f const* y,
  Vec3f const* z,
  float scale,
  float _alpha)
{
  Vec3f left    = Vec3f_Add(*pos, Vec3f_Muls(*x, scale));
  Vec3f up      = Vec3f_Add(*pos, Vec3f_Muls(*y, scale));
  Vec3f forward = Vec3f_Add(*pos, Vec3f_Muls(*z, scale));
  glBegin(GL_LINES);
  glColor4f(1, 0.25f, 0.25f, _alpha);
  glVertex3f(UNPACK3(*pos));
  glVertex3f(UNPACK3(left));
  glColor4f(0.25f, 1, 0.25f, _alpha);
  glVertex3f(UNPACK3(*pos));
  glVertex3f(UNPACK3(up));
  glColor4f(0.25f, 0.25f, 1, _alpha);
  glVertex3f(UNPACK3(*pos));
  glVertex3f(UNPACK3(forward));
  GLCALL(glEnd())

  glBegin(GL_POINTS);
  glColor4f(1, 1, 1, _alpha);
  glVertex3f(UNPACK3(*pos));
  GLCALL(glEnd())
}

void Draw_Border (float s, float x, float y, float w, float h) {
  Draw_Rect(x, y, w, s);
  Draw_Rect(x, y + h - s, w, s);
  Draw_Rect(x, y + s, s, h - 2*s);
  Draw_Rect(x + w - s, y + s, s, h - 2*s);
}

void Draw_Box3 (Box3f const* self) {
  Metric_AddDrawImm(6, 12, 24);
  glBegin(GL_QUADS);
  /* Left. */
  glVertex3f(self->lower.x, self->lower.y, self->lower.z);
  glVertex3f(self->lower.x, self->lower.y, self->upper.z);
  glVertex3f(self->lower.x, self->upper.y, self->upper.z);
  glVertex3f(self->lower.x, self->upper.y, self->lower.z);

  /* Right. */
  glVertex3f(self->upper.x, self->lower.y, self->lower.z);
  glVertex3f(self->upper.x, self->upper.y, self->lower.z);
  glVertex3f(self->upper.x, self->upper.y, self->upper.z);
  glVertex3f(self->upper.x, self->lower.y, self->upper.z);

  /* Front. */
  glVertex3f(self->lower.x, self->lower.y, self->upper.z);
  glVertex3f(self->upper.x, self->lower.y, self->upper.z);
  glVertex3f(self->upper.x, self->upper.y, self->upper.z);
  glVertex3f(self->lower.x, self->upper.y, self->upper.z);

  /* Back. */
  glVertex3f(self->lower.x, self->lower.y, self->lower.z);
  glVertex3f(self->lower.x, self->upper.y, self->lower.z);
  glVertex3f(self->upper.x, self->upper.y, self->lower.z);
  glVertex3f(self->upper.x, self->lower.y, self->lower.z);

  /* Top. */
  glVertex3f(self->lower.x, self->upper.y, self->lower.z);
  glVertex3f(self->lower.x, self->upper.y, self->upper.z);
  glVertex3f(self->upper.x, self->upper.y, self->upper.z);
  glVertex3f(self->upper.x, self->upper.y, self->lower.z);

  /* Bottom. */
  glVertex3f(self->lower.x, self->lower.y, self->lower.z);
  glVertex3f(self->upper.x, self->lower.y, self->lower.z);
  glVertex3f(self->upper.x, self->lower.y, self->upper.z);
  glVertex3f(self->lower.x, self->lower.y, self->upper.z);
  GLCALL(glEnd())
}

void Draw_Clear (float r, float g, float b, float a) {
  GLCALL(glClearColor(r, g, b, a))
  GLCALL(glClear(GL_COLOR_BUFFER_BIT))
}

void Draw_ClearDepth (float d) {
  GLCALL(glClearDepth(d))
  GLCALL(glClear(GL_DEPTH_BUFFER_BIT))
}

void Draw_Color (float r, float g, float b, float a) {
  float alpha = alphaIndex >= 0 ? alphaStack[alphaIndex] : 1;
  color = Vec4f_Create(r, g, b, a);
  GLCALL(glColor4f(r, g, b, a * alpha))
}

void Draw_Flush () {
  Metric_Inc(Metric_Flush);
  GLCALL(glFinish())
}

void Draw_Line (float x1, float y1, float x2, float y2) {
  glBegin(GL_LINES);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  GLCALL(glEnd())
}

void Draw_Line3 (Vec3f const* p1, Vec3f const* p2) {
  glBegin(GL_LINES);
  glVertex3f(UNPACK3(*p1));
  glVertex3f(UNPACK3(*p2));
  GLCALL(glEnd())
}

void Draw_LineWidth (float width) {
  GLCALL(glLineWidth(width))
}

void Draw_Plane (Vec3f const* p, Vec3f const* n, float scale) {
  Vec3f e1 = Abs(n->x) < 0.7f ? Vec3f_Create(1, 0, 0) : Vec3f_Create(0, 1, 0);
  e1 = Vec3f_Normalize(Vec3f_Reject(e1, *n));
  Vec3f e2 = Vec3f_Cross(*n, e1);

  Vec3f p0 = Vec3f_Add(*p, Vec3f_Add(Vec3f_Muls(e1, -scale), Vec3f_Muls(e2, -scale)));
  Vec3f p1 = Vec3f_Add(*p, Vec3f_Add(Vec3f_Muls(e1,  scale), Vec3f_Muls(e2, -scale)));
  Vec3f p2 = Vec3f_Add(*p, Vec3f_Add(Vec3f_Muls(e1,  scale), Vec3f_Muls(e2,  scale)));
  Vec3f p3 = Vec3f_Add(*p, Vec3f_Add(Vec3f_Muls(e1, -scale), Vec3f_Muls(e2,  scale)));

  Metric_AddDrawImm(1, 2, 4);
  glBegin(GL_QUADS);
  glVertex3f(UNPACK3(p0));
  glVertex3f(UNPACK3(p1));
  glVertex3f(UNPACK3(p2));
  glVertex3f(UNPACK3(p3));
  GLCALL(glEnd())
}

void Draw_Point (float x, float y) {
  glBegin(GL_POINTS);
  glVertex2f(x, y);
  GLCALL(glEnd())
}

void Draw_Point3 (float x, float y, float z) {
  glBegin(GL_POINTS);
  glVertex3f(x, y, z);
  GLCALL(glEnd())
}

void Draw_PointSize (float size) {
  GLCALL(glPointSize(size))
}

void Draw_Poly (Vec2f const* points, int count) {
  Metric_AddDrawImm(1, count - 2, count);
  glBegin(GL_POLYGON);
  for (int i = 0; i < count; ++i)
    glVertex2f(UNPACK2(points[i]));
  GLCALL(glEnd());
}

void Draw_Poly3 (Vec3f const* points, int count) {
  Metric_AddDrawImm(1, count - 2, count);
  glBegin(GL_POLYGON);
  for (int i = 0; i < count; ++i)
    glVertex3f(UNPACK3(points[i]));
  GLCALL(glEnd());
}

void Draw_Quad (Vec2f const* p1, Vec2f const* p2, Vec2f const* p3, Vec2f const* p4) {
  Metric_AddDrawImm(1, 2, 4);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex2f(UNPACK2(*p1));
  glTexCoord2f(0, 1); glVertex2f(UNPACK2(*p2));
  glTexCoord2f(1, 1); glVertex2f(UNPACK2(*p3));
  glTexCoord2f(1, 0); glVertex2f(UNPACK2(*p4));
  GLCALL(glEnd())
}

void Draw_Quad3 (Vec3f const* p1, Vec3f const* p2, Vec3f const* p3, Vec3f const* p4) {
  Metric_AddDrawImm(1, 2, 4);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(UNPACK3(*p1));
  glTexCoord2f(0, 1); glVertex3f(UNPACK3(*p2));
  glTexCoord2f(1, 1); glVertex3f(UNPACK3(*p3));
  glTexCoord2f(1, 0); glVertex3f(UNPACK3(*p4));
  GLCALL(glEnd())
}

void Draw_Rect (float x1, float y1, float xs, float ys) {
  float x2 = x1 + xs;
  float y2 = y1 + ys;
  Metric_AddDrawImm(1, 2, 4);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex2f(x1, y1);
  glTexCoord2f(0, 1); glVertex2f(x1, y2);
  glTexCoord2f(1, 1); glVertex2f(x2, y2);
  glTexCoord2f(1, 0); glVertex2f(x2, y1);
  GLCALL(glEnd())
}

void Draw_SmoothLines (bool enabled) {
  if (enabled) {
    GLCALL(glEnable(GL_LINE_SMOOTH))
    GLCALL(glHint(GL_LINE_SMOOTH_HINT, GL_NICEST))
  } else {
    GLCALL(glDisable(GL_LINE_SMOOTH))
    GLCALL(glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST))
  }
}

void Draw_SmoothPoints (bool enabled) {
  if (enabled) {
    GLCALL(glEnable(GL_POINT_SMOOTH))
    GLCALL(glHint(GL_POINT_SMOOTH_HINT, GL_NICEST))
  } else {
    GLCALL(glDisable(GL_POINT_SMOOTH))
    GLCALL(glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST))
  }
}

inline static Vec3f Spherical (float r, float yaw, float pitch) {
  return Vec3f_Create(
    r * Sin(pitch) * Cos(yaw),
    r * Cos(pitch),
    r * Sin(pitch) * Sin(yaw));
}

/* TODO JP : Lazy creation of VBO / IBO & glDraw instead of immediate. */
void Draw_Sphere (Vec3f const* p, float r) {
  const size_t res = 7;
  const float fRes = float(res);

  /* First Row */ {
    Metric_AddDrawImm(res, res, res * 3);
    glBegin(GL_TRIANGLES);
    float lastTheta = float(res - 1) / fRes * Tau;
    float phi = 1.0f / fRes * Pi;
    Vec3f tc = Vec3f_Add(*p, Spherical(r, 0, 0));
    for (size_t iTheta = 0; iTheta < res; iTheta++) {
      float theta = float(iTheta) / fRes * Tau;
      Vec3f br = Vec3f_Add(*p, Spherical(r, lastTheta, phi));
      Vec3f bl = Vec3f_Add(*p, Spherical(r, theta, phi));
      glVertex3f(UNPACK3(br));
      glVertex3f(UNPACK3(tc));
      glVertex3f(UNPACK3(bl));
      lastTheta = theta;
    }
    GLCALL(glEnd())
  }

  /* Middle Rows */ {
    Metric_AddDrawImm(res - 2, 2 * (res - 2), 4 * (res - 2));
    glBegin(GL_QUADS);
    float lastPhi = 1.0f / fRes * Pi;
    float lastTheta = float(res - 1) / fRes * Tau;

    for (size_t iPhi = 2; iPhi < res; iPhi++) {
      float phi = float(iPhi) / fRes * Pi;
      for (size_t iTheta = 0; iTheta < res; iTheta++) {
        float theta = float(iTheta) / fRes * Tau;
        Vec3f br = Vec3f_Add(*p, Spherical(r, lastTheta, phi));
        Vec3f tr = Vec3f_Add(*p, Spherical(r, lastTheta, lastPhi));
        Vec3f tl = Vec3f_Add(*p, Spherical(r, theta, lastPhi));
        Vec3f bl = Vec3f_Add(*p, Spherical(r, theta, phi));
        glVertex3f(UNPACK3(br));
        glVertex3f(UNPACK3(tr));
        glVertex3f(UNPACK3(tl));
        glVertex3f(UNPACK3(bl));
        lastTheta = theta;
      }
      lastPhi = phi;
    }
    GLCALL(glEnd())
  }

  /* Bottom Row */ {
    Metric_AddDrawImm(res, res, res * 3);
    glBegin(GL_TRIANGLES);
    float lastTheta = float(res - 1) / fRes * Tau;
    float phi = float(res - 1) / fRes * Pi;
    Vec3f bc = Vec3f_Add(*p, Spherical(r, 0, Pi));

    for (size_t iTheta = 0; iTheta < res; iTheta++) {
      float theta = float(iTheta) / fRes * Tau;
      Vec3f tr = Vec3f_Add(*p, Spherical(r, lastTheta, phi));
      Vec3f tl = Vec3f_Add(*p, Spherical(r, theta, phi));
      glVertex3f(UNPACK3(tr));
      glVertex3f(UNPACK3(tl));
      glVertex3f(UNPACK3(bc));
      lastTheta = theta;
    }
    GLCALL(glEnd())
  }
}

void Draw_Tri (Vec2f const* v1, Vec2f const* v2, Vec2f const* v3) {
  Metric_AddDrawImm(1, 1, 3);
  glBegin(GL_TRIANGLES);
  glTexCoord2f(0, 0); glVertex2f(UNPACK2(*v1));
  glTexCoord2f(0, 1); glVertex2f(UNPACK2(*v2));
  glTexCoord2f(1, 1); glVertex2f(UNPACK2(*v3));
  GLCALL(glEnd())
}

void Draw_Tri3 (Vec3f const* v1, Vec3f const* v2, Vec3f const* v3) {
  Metric_AddDrawImm(1, 1, 3);
  glBegin(GL_TRIANGLES);
  glTexCoord2f(0, 0); glVertex3f(UNPACK3(*v1));
  glTexCoord2f(0, 1); glVertex3f(UNPACK3(*v2));
  glTexCoord2f(1, 1); glVertex3f(UNPACK3(*v3));
  GLCALL(glEnd())
}

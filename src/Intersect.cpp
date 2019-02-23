#include "Intersect.h"
#include "Matrix.h"
#include "Plane.h"
#include "Polygon.h"
#include "Ray.h"
#include "Sphere.h"
#include "PhxMath.h"
#include "Triangle.h"
#include "Vec3.h"
#include "Vec4.h"

/* NOTE: On Epsilons
 *  - PLANE_THICKNESS_EPSILON
 *
 *    1) Limited by floating point precision.
 *    Before building a BSP we normalize the mesh by dividing by the bounds
 *    size such that vertex positions are between [-1, 1] on all axes. When
 *    choosing splitting planes we call Polygon_ToPlane for autopartitioning
 *    cuts. As the vertices and center of the triangle can be anywhere in that
 *    [-1, 1] range, our precision is capped around 1e-7. Even if all inputs
 *    are double precision, simply doing the vertex-plane distance calculation
 *    in float will end up rounding the distance to somewhere around 1e-8 in
 *    many cases.
 *
 *    2) Limited by overall BSP size (in bytes)
 *    The smaller the epsilon is, the more eagerly we end up cutting polygons.
 *    In order to make a cut, an edge must cross from 'in front' of the plane
 *    to 'behind' the plane. The thinner the plane is, the easier it is to
 *    satisfy this condition and we end up making a lot more cuts, which leads
 *    to more triangles, which leads to more memory usage. At last checl (r631)
 *    Dropping from 1e-4 to 1e-5 raised the overall memory usage by 25% while
 *    only gaining 0.1 us.
 */

/* TODO : Need to handle epsilons properly in these intersection tests */

bool Intersect_PointBox (Matrix* src, Matrix* dst) {
  Matrix* inv = Matrix_Inverse(dst);
  Vec3f srcPt; Matrix_GetPos(src, &srcPt);
  Vec3f dstPt; Matrix_MulPoint(inv, &dstPt, UNPACK3(srcPt));
  Matrix_Free(inv);
  return
    -1.0f < dstPt.x && dstPt.x < 1.0f &&
    -1.0f < dstPt.y && dstPt.y < 1.0f &&
    -1.0f < dstPt.z && dstPt.z < 1.0f;
}

bool Intersect_PointTriangle_Barycentric (Vec3f const* p, Triangle const* tri) {
  Vec3f const* v = tri->vertices;

  Vec3f pv0 = Vec3f_Sub(v[0], *p);
  Vec3f pv1 = Vec3f_Sub(v[1], *p);
  Vec3f pv2 = Vec3f_Sub(v[2], *p);

  /* NOTE: Any scale factor on plane.n will fall out of the calcuations for A and B. */
  Plane plane; Triangle_ToPlaneFast(tri, &plane);

  float areaABC = Vec3f_Dot(plane.n, plane.n);
  float areaPBC = Vec3f_Dot(plane.n, Vec3f_Cross(pv1, pv2));
  float areaPCA = Vec3f_Dot(plane.n, Vec3f_Cross(pv2, pv0));

  float A = areaPBC / areaABC;
  float B = areaPCA / areaABC;
  float C = 1.0f - A - B;

  /* TODO : Need a proper epsilon */
  float fuzzyMin = 0.0f - 0.01f;
  float fuzzyMax = 1.0f + 0.01f;

  return A > fuzzyMin && A < fuzzyMax
    && B > fuzzyMin && B < fuzzyMax
    && C > fuzzyMin && C < fuzzyMax;
}

bool Intersect_RayPlane (
  Ray const* ray,
  Plane const* plane,
  Vec3f* pHit)
{
  /* TODO : Shouldn't we handle denom == 0? */
  float dist = plane->d - Vec3f_Dot(plane->n, ray->p);
  float denom = Vec3f_Dot(plane->n, ray->dir);
  float t = dist / denom;

  if (t >= ray->tMin && t <= ray->tMax) {
    *pHit = Vec3f_Add(ray->p, Vec3f_Muls(ray->dir, t));
    return true;
  }

  return false;
}

bool Intersect_RayTriangle_Barycentric (
  Ray const* ray,
  Triangle const* tri,
  float tEpsilon,
  float* tHit)
{
  /* NOTE: Any scale factor on plane.n falls out of the calculation for t.
   * denom and dist are both off by the scale factor, but we don't need
   * them for anything other than t.
   */
  Plane plane; Triangle_ToPlaneFast(tri, &plane);

  float dist = Vec3f_Dot(plane.n, ray->p) - plane.d;
  float denom = -Vec3f_Dot(plane.n, ray->dir);

  if (denom != 0.0f) {
    float t =  dist / denom;
    if (t > ray->tMin - tEpsilon && t < ray->tMax + tEpsilon) {
      Vec3f const* v = tri->vertices;
      Vec3f p; Ray_GetPoint(ray, t, &p);

      Vec3f pv0 = Vec3f_Sub(v[0], p);
      Vec3f pv1 = Vec3f_Sub(v[1], p);
      Vec3f pv2 = Vec3f_Sub(v[2], p);

      float areaABC = Vec3f_Dot(plane.n, plane.n);
      float areaPBC = Vec3f_Dot(plane.n, Vec3f_Cross(pv1, pv2));
      float areaPCA = Vec3f_Dot(plane.n, Vec3f_Cross(pv2, pv0));

      float A = areaPBC / areaABC;
      float B = areaPCA / areaABC;
      float C = 1.0f - A - B;

      /* TODO : Need a proper epsilon */
      float fuzzyMin = 0.0f - 0.01f;
      float fuzzyMax = 1.0f + 0.01f;

      if (A > fuzzyMin && A < fuzzyMax &&
          B > fuzzyMin && B < fuzzyMax &&
          C > fuzzyMin && C < fuzzyMax) {
        *tHit = t;
        return true;
      }
    }
    else {
      /* TODO : Handle parallel but in triangle (or its thick plane) */
    }
  }

  return false;
}

/* http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/raytri/ */
bool Intersect_RayTriangle_Moller1 (
  Ray const* ray,
  Triangle const* tri,
  float* tHit)
{
  /* Rewritten test sign of determinant; division is at the end. */
  Vec3f const* vt = tri->vertices;
  Vec3f edge1 = Vec3f_Sub(vt[1], vt[0]);
  Vec3f edge2 = Vec3f_Sub(vt[2], vt[0]);

  float u, v;
  Vec3f qvec;

  /* Begin calculating determinant - also used to calculate U parameter. */
  Vec3f pvec = Vec3f_Cross(ray->dir, edge2);

  /* TODO : Need a proper epsilon */
  const float epsilon = .000001f;

  /* If determinant is near zero, ray lies in plane of triangle. */
  float det = Vec3f_Dot(edge1, pvec);
  if (det > epsilon) {
    /* Calculate distance from vert0 to ray origin. */
    Vec3f tvec = Vec3f_Sub(ray->p, vt[0]);

    /* Calculate U parameter and test bounds. */
    u = Vec3f_Dot(tvec, pvec);
    if (u < 0.0 || u > det)
      return false;

    /* Prepare to test V parameter. */
    qvec = Vec3f_Cross(tvec, edge1);

    /* Calculate V parameter and test bounds. */
    v = Vec3f_Dot(ray->dir, qvec);

    if (v < 0.0 || (u + v) > det)
      return false;
  }

  else if (det < -epsilon) {
    /* Calculate distance from vert0 to ray origin. */
    Vec3f tvec = Vec3f_Sub(ray->p, vt[0]);

    /* Calculate U parameter and test bounds. */
    u = Vec3f_Dot(tvec, pvec);
    if (u > 0.0 || u < det)
      return false;

    /* Prepare to test V parameter. */
    qvec = Vec3f_Cross(tvec, edge1);

    /* Calculate V parameter and test bounds. */
    v = Vec3f_Dot(ray->dir, qvec);

    if (v > 0.0 || u + v < det)
      return false;
  }

  else {
    /* Ray is parallel to the plane of the triangle */
    return false;
  }

  float inv_det = 1.0f / det;

  /* Ray intersects; calculate t. */
  *tHit = Vec3f_Dot(edge2, qvec) * inv_det;
  return true;
}

/* http://www.cs.virginia.edu/~gfx/courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf */
bool Intersect_RayTriangle_Moller2 (
  Ray const* ray,
  Triangle const* tri,
  float* tHit)
{
  Vec3f const* vt = tri->vertices;

  /* Find vectors for two edges sharing vert. */
  Vec3f edge1 = Vec3f_Sub(vt[1], vt[0]);
  Vec3f edge2 = Vec3f_Sub(vt[2], vt[0]);

  /* Begin calculating determinant - also used to calculate U parameter. */
  Vec3f pvec = Vec3f_Cross(ray->dir, edge2);

  /* If determinant is near zero ray lies in plane of triangle. */
  float det = Vec3f_Dot(edge1, pvec);

  /* TODO : Need a proper epsilon */
  if (Abs(det) < 0.000001f)
    return false;

  float inv_det = 1.0f / det;

  /* Calculate distance from vert to ray origin. */
  Vec3f tvec = Vec3f_Sub(ray->p, vt[0]);

  /* TODO : Need a proper epsilon */
  float fuzzyMin = 0.0f - 0.01f;
  float fuzzyMax = 1.0f + 0.01f;

  /* Calculate U and test bounds. */
  float u = Vec3f_Dot(tvec, pvec) * inv_det;
  if (u < fuzzyMin || u > fuzzyMax)
    return false;

  /* Prepare to test V. */
  Vec3f qvec = Vec3f_Cross(tvec, edge1);

  /* Calculate V and test bounds. */
  float v = Vec3f_Dot(ray->dir, qvec) * inv_det;
  if (v < fuzzyMin || (u + v) > fuzzyMax)
    return false;

  /* Ray intersects; calculate t. */
  *tHit = Vec3f_Dot(edge2, qvec) * inv_det;
  return true;
}

bool Intersect_LineSegmentPlane (
  LineSegment const* lineSegment,
  Plane const* plane,
  Vec3f* pHit)
{
  Vec3f dir = Vec3f_Sub(lineSegment->p1, lineSegment->p0);
  Ray ray = { lineSegment->p0, dir, 0.0f, 1.0f };
  return Intersect_RayPlane(&ray, plane, pHit);
}

bool Intersect_RectRect (Vec4f const* a, Vec4f const* b) {
  Vec4f a2 = { a->x + Min(a->z, 0.0f), a->y + Min(a->w, 0.0f), Abs(a->z), Abs(a->w) };
  Vec4f b2 = { b->x + Min(b->z, 0.0f), b->y + Min(b->w, 0.0f), Abs(b->z), Abs(b->w) };
  return Intersect_RectRectFast(&a2, &b2);
}

bool Intersect_RectRectFast (Vec4f const* a, Vec4f const* b) {
  bool result = true;
  result &= a->x < b->x + b->z;
  result &= b->x < a->x + a->z;
  result &= a->y < b->y + b->w;
  result &= b->y < a->y + a->w;
  return result;
}

/* Realtime Collision Detection, pp 141-142 */
inline static Vec3f ClosestPoint_PointToTriangle (
  Vec3f const* p,
  Triangle const* tri)
{
  Vec3f a = tri->vertices[0];
  Vec3f b = tri->vertices[1];
  Vec3f c = tri->vertices[2];

  /* Check if P in vertex region outside A */
  Vec3f ab = Vec3f_Sub(b,  a);
  Vec3f ac = Vec3f_Sub(c,  a);
  Vec3f ap = Vec3f_Sub(*p, a);
  float d1 = Vec3f_Dot(ab, ap);
  float d2 = Vec3f_Dot(ac, ap);
  if (d1 <= 0.0f && d2 <= 0.0f) return a; // (1, 0, 0)

  /* Check if P in vertex region outside B. */
  Vec3f bp = Vec3f_Sub(*p, b);
  float d3 = Vec3f_Dot(ab, bp);
  float d4 = Vec3f_Dot(ac, bp);
  if (d3 >= 0.0f && d4 <= d3) return b; // (0, 1, 0)

  /* Check if P in edge region of AB, if so return projection of P onto AB. */
  float vc = d1*d4 - d3*d2;
  if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
    float v = d1 / (d1 - d3);
    return Vec3f_Add(a , Vec3f_Muls(ab, v)); // (1 - v, v, 0)
  }

  /* Check if P in vertex region outside C. */
  Vec3f cp = Vec3f_Sub(*p, c);
  float d5 = Vec3f_Dot(ab, cp);
  float d6 = Vec3f_Dot(ac, cp);
  if (d6 >= 0.0f && d5 <= d6) return c; // (0, 0, 1)

  /* Check if P in edge region of AC, if so return projection of P onto AC. */
  float vb = d5*d2 - d1*d6;
  if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
    float w = d2 / (d2 - d6);
    return Vec3f_Add(a, Vec3f_Muls(ac, w)); // (1 - w, 0, w)
  }

  /* Check if P in edge region of BC, if so return projection of P onto BC. */
  float va = d3*d6 - d5*d4;
  float d4m3 = d4 - d3;
  float d5m6 = d5 - d6;
  if (va <= 0.0f && d4m3 >= 0.0f && d5m6 >= 0.0f) {
    float w = d4m3 / (d4m3 + d5m6);
    Vec3f bc = Vec3f_Sub(c, b);
    return Vec3f_Add(b, Vec3f_Muls(bc, w)); // (0, 1 - w, w)
  }

  /* P inside face region. Compute barycentric coordinates (1 - v - w, v, w) */
  float denom = 1.0f / (va + vb + vc);
  float v = vb * denom;
  float w = vc * denom;
  return Vec3f_Add(Vec3f_Add(a, Vec3f_Muls(ab, v)), Vec3f_Muls(ac, w));
}

bool Intersect_SphereTriangle (
  Sphere const* sphere,
  Triangle const* triangle,
  Vec3f* pHit)
{
  Vec3f pClosest = ClosestPoint_PointToTriangle(&sphere->p, triangle);
  float distSq = Vec3f_DistanceSquared(sphere->p, pClosest);
  if (distSq < sphere->r * sphere->r) {
    *pHit = pClosest;
    return true;
  }
  return false;
}

#if 0
/* TODO : This is not yet working properly
 * TODO : Need to precompute index of the largest normal component */
bool Intersect_RayTriangle_Badouel (Ray* ray, Triangle* triangle, float tEpsilon, float* tHit) {
  /* NOTE: There's some great information in the following reference, including
   * comparisons of multiple algorithms and code for each.
   * http://erich.realtimerendering.com/ptinpoly/
   */

  Plane plane = Triangle_ToPlaneFast(triangle);

  float dist = Vec3f_Dot(plane.n, ray->p) - plane.d;
  float denom = -Vec3f_Dot(plane.n, ray->dir);

  if (denom != 0.0f) {
    float t =  dist / denom;

    if (t > ray->tMin - tEpsilon && t < ray->tMax + tEpsilon) {
      Vec3f* pgon = triangle->vertices;
      Vec3f point = Ray_GetPoint(ray, t);

      Vec3f pg1, pg2;
      float tx, ty, u0, u1, u2, v0, v1, vx0, vy0, alpha, beta, denom2;
      int inside_flag;

      tx = point.x;
      ty = point.y;
      vx0 = pgon[0].x;
      vy0 = pgon[0].y;
      u0 = tx - vx0;
      v0 = ty - vy0;

      inside_flag = 0;

      /* TODO : v1 is uninitialized in the first if */
      v1 = 0;

      pg1 = pgon[1];
      pg2 = pgon[2];
      u1 = pg1.x - vx0;
      if (u1 == 0.0) {
        /* 0 and 1 vertices have same X value */

        u2 = pg2.x - vx0;
        if (
          /* compute beta and check bounds */
          ((beta = u0 / u2) < -0.01f) || (beta > 1.01f) ||

          /* compute alpha and check bounds */
          ((alpha = (v0 - beta * (pg2.y - vy0)) / v1) < 0.0)) {

          /* whew! missed! */
          return inside_flag != 0;
        }
      }
      else {
        /* 0 and 1 vertices have different X value */

        /* compute denom2 */
        u2 = pg2.x - vx0;
        v1 = pg1.y - vy0;
        denom2 = (pg2.y - vy0) * u1 - u2 * v1;
        if (
          /* compute beta and check bounds */
          ((beta = (v0 * u1 - u0 * v1) / denom2) < -0.01f) || (beta > 1.01f) ||

          /* compute alpha & check bounds */
          ((alpha = (u0 - beta * u2) / u1) < 0.0)) {

          /* whew! missed! */
          return inside_flag != 0;
        }
      }

      /* check gamma */
      if (alpha + beta <= 1.01f) {
        /* survived */
        inside_flag = !inside_flag;
      }

      return inside_flag != 0;
    }
  }
  else {
    /* TODO : Handle parallel but in triangle (or its thick plane) */
  }

  /* TODO : Coalesce returns? */
  return false;
}

#endif

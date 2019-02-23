#include "Array.h"
#include "Intersect.h"
#include "LineSegment.h"
#include "Plane.h"
#include "Polygon.h"
#include "Ray.h"
#include "Triangle.h"

inline static bool PointsInPlane (Plane* plane, Polygon* polygon) {
  Vec3f* v    = ArrayList_GetData(polygon->vertices);
  int32  vLen = ArrayList_GetSize(polygon->vertices);

  for (int32 i = 0; i < vLen; i++) {
    float dist = Vec3f_Dot(plane->n, v[i]) - plane->d;
    if (Abs(dist) >= PLANE_THICKNESS_EPSILON) return false;
  }

  return true;
};

void Polygon_ToPlane (Polygon* polygon, Plane* out) {
  //NOTE: Newell's method, Realtime Collision Detection p494

  //OPTIMIZE: Could store planes with each polygon in the mesh. Since we are
  //generating them it seems dumb to hand off the polygons and make the BSP
  //re-calculate planes. These can eventually be thrown away to save memory.

  //OPTIMIZE: For offline meshes (if these will exist) the vertices can be
  //ordered such that the cross product of the first 3 gives a reasonable normal.

  Vec3f* v    = ArrayList_GetData(polygon->vertices);
  int32  vLen = ArrayList_GetSize(polygon->vertices);

  Vec3d n = { 0 };
  Vec3d centroid = { 0 };

  Vec3d vCur = Vec3f_ToVec3d(v[vLen - 1]);
  for (int32 i = 0; i < vLen; i++) {
    Vec3d vPrev = vCur;
    vCur = Vec3f_ToVec3d(v[i]);

    n.x += (vPrev.y - vCur.y) * (vPrev.z + vCur.z);
    n.y += (vPrev.z - vCur.z) * (vPrev.x + vCur.x);
    n.z += (vPrev.x - vCur.x) * (vPrev.y + vCur.y);
    centroid = Vec3d_Add(centroid, vCur);
  }
  n = Vec3d_Normalize(n);
  centroid = Vec3d_Divs(centroid, (double) vLen);

  out->n = Vec3d_ToVec3f(n);
  out->d = (float) Vec3d_Dot(centroid, n);

  CHECK2(Assert(PointsInPlane(out, polygon)));
}

void Polygon_ToPlaneFast (Polygon* polygon, Plane* out) {
  //NOTE: Doesn't normalize n and uses v[0] as the center.

  Vec3f* v = ArrayList_GetPtr(polygon->vertices, 0);
  int32 vLen = ArrayList_GetSize(polygon->vertices);

  Vec3f n = { 0 };
  for (int32 i = vLen - 1, j = 0; j < vLen; i = j, j++) {
    n.x += (v[i].y - v[j].y) * (v[i].z + v[j].z);
    n.y += (v[i].z - v[j].z) * (v[i].x + v[j].x);
    n.z += (v[i].x - v[j].x) * (v[i].y + v[j].y);
  }

  out->n = n;
  out->d = Vec3f_Dot(v[0], n);

  CHECK2(Assert(PointsInPlane(out, polygon)));
}

inline static void Polygon_SplitImpl (Polygon* polygon, Plane splitPlane, Polygon* back, Polygon* front) {
  Vec3f a = ArrayList_GetLast(polygon->vertices);
  PointClassification aSide = Plane_ClassifyPoint(&splitPlane, &a);

  for (int32 j = 0; j < ArrayList_GetSize(polygon->vertices); j++) {
    Vec3f b = ArrayList_Get(polygon->vertices, j);
    PointClassification bSide = Plane_ClassifyPoint(&splitPlane, &b);

    if (bSide == PointClassification_InFront) {
      if (aSide == PointClassification_Behind) {
        Vec3f i;
        LineSegment lineSegment = {b, a};
        bool hit = Intersect_LineSegmentPlane(&lineSegment, &splitPlane, &i);
        ArrayList_Append(front->vertices, i);
        ArrayList_Append(back->vertices, i);

        Assert(hit); UNUSED(hit);
        Assert(Plane_ClassifyPoint(&splitPlane, &i) == PointClassification_Coplanar);
      }
      ArrayList_Append(front->vertices, b);
    }
    else if (bSide == PointClassification_Behind) {
      if (aSide == PointClassification_InFront) {
        Vec3f i;
        LineSegment lineSegment = {a, b};
        bool hit = Intersect_LineSegmentPlane(&lineSegment, &splitPlane, &i);
        ArrayList_Append(front->vertices, i);
        ArrayList_Append(back->vertices, i);

        Assert(hit); UNUSED(hit);
        Assert(Plane_ClassifyPoint(&splitPlane, &i) == PointClassification_Coplanar);
      }
      else if (aSide == PointClassification_Coplanar) {
        ArrayList_Append(back->vertices, a);
      }
      ArrayList_Append(back->vertices, b);
    }
    else {
      if (aSide == PointClassification_Behind) {
        ArrayList_Append(back->vertices, b);
      }
      ArrayList_Append(front->vertices, b);
    }

    a = b;
    aSide = bSide;
  }
}

void Polygon_SplitSafe (Polygon* polygon, Plane splitPlane, Polygon* back, Polygon* front) {
  Polygon_SplitImpl(polygon, splitPlane, back, front);

  Polygon* polygons[] = { front, back };
  for (int i = 0; i < Array_GetSize(polygons); ++i) {
    Polygon* polygonPart = polygons[i];
    Vec3f* v    = ArrayList_GetData(polygonPart->vertices);
    int32  vLen = ArrayList_GetSize(polygonPart->vertices);

    Vec3f  vCur = v[vLen - 1];
    for (int32 l = 0; l < vLen; ++l) {
      Vec3f vPrev = vCur;
      vCur = v[l];

      float edgeLen = Vec3f_Length(Vec3f_Sub(vCur, vPrev));
      if (edgeLen < 0.75f * PLANE_THICKNESS_EPSILON) {
        /* TODO : Stick one warning at the end of the build process. */
        //CHECK2(Warn("Polygon_SplitSafe: Split produced a degenerate or sliver polygon. Sending it to both sides."));
        ArrayList_Clear(back->vertices);
        ArrayList_Clear(front->vertices);
        ArrayList_ForEach(polygon->vertices, Vec3f, vertex) {
          ArrayList_Append(back->vertices,  *vertex);
          ArrayList_Append(front->vertices, *vertex);
        }
        return;
      }
    }
  }
}

void Polygon_Split (Polygon* polygon, Plane splitPlane, Polygon* back, Polygon* front) {
  Polygon_SplitImpl(polygon, splitPlane, back, front);

  CHECK3 (
    Polygon_Validate(back);
    Polygon_Validate(front);
  )
}

void Polygon_GetCentroid (Polygon* polygon, Vec3f* out) {
  Vec3f centroid = { 0 };

  ArrayList_ForEach(polygon->vertices, Vec3f, v) {
    Vec3f_IAdd(&centroid, *v);
  }
  Vec3f_IDivs(&centroid, (float) ArrayList_GetSize(polygon->vertices));

  *out = centroid;
}

void Polygon_ConvexToTriangles (Polygon* polygon, int32* triangles_capacity, int32* triangles_size, Triangle** triangles_data) {
  Vec3f* v = ArrayList_GetData(polygon->vertices);
  int32 vLen = ArrayList_GetSize(polygon->vertices);

  for (int32 i = 1; i < vLen - 1; i++) {
    /* TODO : Do this all at once */
    IF_UNLIKELY (*triangles_capacity == *triangles_size) {
      *triangles_capacity = *triangles_capacity ? *triangles_capacity * 2 : 1;
      *triangles_data = (Triangle*)MemRealloc(*triangles_data, *triangles_capacity * sizeof(Triangle));
    }

    Triangle* triangle = *triangles_data + *triangles_size;
    (*triangles_size)++;
    triangle->vertices[0] = v[0];
    triangle->vertices[1] = v[i];
    triangle->vertices[2] = v[i + 1];
  }
}

Error Polygon_Validate (Polygon* polygon) {
  Vec3f* v    = ArrayList_GetData(polygon->vertices);
  int32  vLen = ArrayList_GetSize(polygon->vertices);

  Vec3f vCur = v[vLen - 1];
  for (int32 i = 0; i < vLen; i++) {
    Vec3f vPrev = vCur;
    vCur = v[i];

    //NaN or Inf
    Error e = Vec3f_Validate(vCur);
    if (e != Error_None) return Error_VertPos | e;

    //Degenerate
    for (int32 j = i + 1; j < vLen; j++) {
      if (Vec3f_Equal(vCur, v[j])) return Error_VertPos | Error_Degenerate;
    }

    //Sliver
    /* TODO : See comment on slivers in Triangle_Validate */
    float edgeLen = Vec3f_Length(Vec3f_Sub(vCur, vPrev));
    if (edgeLen < 0.75f*PLANE_THICKNESS_EPSILON) return Error_VertPos | Error_Underflow;
  }

  return Error_None;
}

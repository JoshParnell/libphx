#include "Intersect.h"
#include "Plane.h"
#include "Polygon.h"

PointClassification Plane_ClassifyPoint (Plane* plane, Vec3f* p) {
  float magnitude = Abs(1.0f - Vec3f_Length(plane->n));
  Assert(magnitude < .01f); UNUSED(magnitude);

  float dist = Vec3f_Dot(plane->n, *p) - plane->d;
  if (dist > PLANE_THICKNESS_EPSILON)
    return PointClassification_InFront;
  else if (dist < -PLANE_THICKNESS_EPSILON)
    return PointClassification_Behind;
  else
    return PointClassification_Coplanar;
}

PolygonClassification Plane_ClassifyPolygon (Plane* plane, Polygon* polygon) {
  int32 numInFront = 0;
  int32 numBehind  = 0;

  for (int32 i = 0; i < ArrayList_GetSize(polygon->vertices); i++) {
    Vec3f vertex = ArrayList_Get(polygon->vertices, i);
    PointClassification classification = Plane_ClassifyPoint(plane, &vertex);
    switch (classification) {
      default: Fatal("Plane_ClassifyPolygon: Unhandled case: %i", classification);

      case PointClassification_InFront:
        numInFront++;
        break;

      case PointClassification_Behind:
        numBehind++;
        break;

      case PointClassification_Coplanar:
        //Doesn't count for either
        break;
    }

    /* TODO : This early out may not make as much sense if the BSP stops cutting triangles. */
    if (numInFront != 0 && numBehind != 0)
      return PolygonClassification_Straddling;
  }

  if (numInFront != 0)
    return PolygonClassification_InFront;
  if (numBehind != 0)
    return PolygonClassification_Behind;
  return PolygonClassification_Coplanar;
}

Error Plane_Validate (Plane* plane) {
  Error e = Error_None;

  e |= Float_Validate(plane->d);
  e |= Vec3f_Validate(plane->n);

  return e;
}

void Plane_FromPolygon (Polygon* polygon, Plane* plane) {
  Polygon_ToPlane(polygon, plane);
}

void Plane_FromPolygonFast (Polygon* polygon, Plane* plane) {
  Polygon_ToPlaneFast(polygon, plane);
}

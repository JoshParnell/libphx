#ifndef PHX_Plane
#define PHX_Plane

#include "Vec3.h"

struct Plane {
  Vec3f n;
  float d;
};

const PointClassification PointClassification_InFront  = 1;
const PointClassification PointClassification_Behind   = 2;
const PointClassification PointClassification_Coplanar = 3;

const PolygonClassification PolygonClassification_InFront    = 1;
const PolygonClassification PolygonClassification_Behind     = 2;
const PolygonClassification PolygonClassification_Coplanar   = 3;
const PolygonClassification PolygonClassification_Straddling = 4;

PHX_API PointClassification    Plane_ClassifyPoint    (Plane*, Vec3f*);
PHX_API PolygonClassification  Plane_ClassifyPolygon  (Plane*, Polygon*);
PHX_API Error                  Plane_Validate         (Plane*);

PHX_API void                   Plane_FromPolygon      (Polygon*, Plane*);
PHX_API void                   Plane_FromPolygonFast  (Polygon*, Plane*);

#endif

#ifndef PHX_Polygon
#define PHX_Polygon

#include "ArrayList.h"
#include "Common.h"
#include "Vec3.h"

struct Polygon {
  ArrayList(Vec3f, vertices);
};

PHX_API void   Polygon_ToPlane            (Polygon*, Plane*);
PHX_API void   Polygon_ToPlaneFast        (Polygon*, Plane*);
PHX_API void   Polygon_Split              (Polygon*, Plane splitPlane, Polygon* back, Polygon* front);
PHX_API void   Polygon_SplitSafe          (Polygon*, Plane splitPlane, Polygon* back, Polygon* front);
PHX_API void   Polygon_GetCentroid        (Polygon*, Vec3f*);
PHX_API void   Polygon_ConvexToTriangles  (Polygon*, int32* triangles_capacity, int32* triangles_size, Triangle** triangles_data);
PHX_API Error  Polygon_Validate           (Polygon*);

#endif

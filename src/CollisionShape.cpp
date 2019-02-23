#include "ArrayList.h"
#include "CollisionShape.h"
#include "Mesh.h"
#include "Vertex.h"

struct Cache {
  ArrayList(CollisionShape, shapes);
} static cache;

CollisionShape* CollisionShape_GetCached (int index) {
  return ArrayList_GetPtr(cache.shapes, index);
}

CollisionShape* CollisionShape_Create (CollisionShape shape) {
  Assert(shape.scale > 0);
  switch (shape.type) {
    default:
      Fatal("CollisionShape_CreateShape: Unhandled case for CollisionShapeType %i.", shape.type);
      break;

    case CollisionShapeType_Sphere: {
      Assert(shape.sphere.radius > 0);
      ArrayList_ForEachI(cache.shapes, i) {
        CollisionShape* cached = ArrayList_GetPtr(cache.shapes, i);
        if (cached->type == shape.type &&
            cached->scale == shape.scale &&
            cached->sphere.radius == shape.sphere.radius)
          return cached;
      }

      shape.base.handle = new btSphereShape(shape.sphere.radius);
      shape.base.handle->setLocalScaling(btVector3(REPEAT3(shape.scale)));
      break;
    }

    case CollisionShapeType_Box: {
      Assert(shape.box.halfExtents.x > 0);
      Assert(shape.box.halfExtents.y > 0);
      Assert(shape.box.halfExtents.z > 0);
      ArrayList_ForEachI(cache.shapes, i) {
        CollisionShape* cached = ArrayList_GetPtr(cache.shapes, i);
        if (cached->type == shape.type &&
            cached->scale == shape.scale &&
            Vec3f_Equal(cached->box.halfExtents, shape.box.halfExtents))
          return cached;
      }

      shape.base.handle = new btBoxShape(Vec3f_ToBullet(&shape.box.halfExtents));
      shape.base.handle->setLocalScaling(btVector3(REPEAT3(shape.scale)));
      break;
    }

    case CollisionShapeType_Hull: {
      Assert(shape.hull.mesh != 0);
      btConvexHullShape* hullHandle = 0;
      ArrayList_ForEachI(cache.shapes, i) {
        CollisionShape* cached = ArrayList_GetPtr(cache.shapes, i);
        if (cached->type == shape.type &&
            cached->hull.mesh == shape.hull.mesh) {
          hullHandle = cached->hull.hullHandle;
          break;
        }
      }

      if (!hullHandle) {
        Vertex* vFirst = Mesh_GetVertex(shape.hull.mesh, 0);
        int     vCount = Mesh_GetVertexCount(shape.hull.mesh);
        Assert(vCount != 0);
        /* TODO : Don't leak these when we actually start removing shapes from the cache. */
        hullHandle = new btConvexHullShape(&(vFirst->p.x), vCount, sizeof(Vertex));
        hullHandle->optimizeConvexHull();
      }

      shape.hull.hullHandle = hullHandle;
      shape.base.handle = new btUniformScalingShape(hullHandle, shape.scale);
      break;
    }

    case CollisionShapeType_Compound: {
      shape.base.handle = new btCompoundShape(true, 4);
      shape.base.handle->setLocalScaling(btVector3(REPEAT3(shape.scale)));
      break;
    }
  }

  shape.iShape = ArrayList_GetSize(cache.shapes);
  ArrayList_Append(cache.shapes, shape);
  return ArrayList_GetLastPtr(cache.shapes);
}

void CollisionShape_Free (CollisionShape*) {
  /* TODO: Actually free shapes. */
}

CollisionShape* CollisionShape_CreateBox (Vec3f* halfExtents) {
  CollisionShape shapeDef = {};
  shapeDef.scale           = 1.0f;
  shapeDef.type            = CollisionShapeType_Box;
  shapeDef.box.halfExtents = *halfExtents;
  return CollisionShape_Create(shapeDef);
}

CollisionShape* CollisionShape_CreateBoxFromMesh (Mesh* mesh) {
  Box3f bounds = {}; Mesh_GetBound(mesh, &bounds);

  CollisionShape shapeDef = {};
  shapeDef.scale             = 1.0f;
  shapeDef.type              = CollisionShapeType_Box;
  shapeDef.box.halfExtents.x = Max(Abs(bounds.upper.x), Abs(bounds.lower.x));
  shapeDef.box.halfExtents.y = Max(Abs(bounds.upper.y), Abs(bounds.lower.y));
  shapeDef.box.halfExtents.z = Max(Abs(bounds.upper.z), Abs(bounds.lower.z));
  return CollisionShape_Create(shapeDef);
}

CollisionShape* CollisionShape_CreateSphere (float radius) {
  CollisionShape shapeDef = {};
  shapeDef.scale         = 1.0f;
  shapeDef.type          = CollisionShapeType_Sphere;
  shapeDef.sphere.radius = radius;
  return CollisionShape_Create(shapeDef);
}

CollisionShape* CollisionShape_CreateSphereFromMesh (Mesh* mesh) {
  CollisionShape shapeDef = {};
  shapeDef.scale         = 1.0f;
  shapeDef.type          = CollisionShapeType_Sphere;
  shapeDef.sphere.radius = Mesh_GetRadius(mesh);
  return CollisionShape_Create(shapeDef);
}

CollisionShape* CollisionShape_CreateHullFromMesh (Mesh* mesh) {
  CollisionShape shapeDef = {};
  shapeDef.scale     = 1.0f;
  shapeDef.type      = CollisionShapeType_Hull;
  shapeDef.hull.mesh = mesh;
  return CollisionShape_Create(shapeDef);
}

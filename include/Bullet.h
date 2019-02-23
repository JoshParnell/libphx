#ifndef PHX_Bullet
#define PHX_Bullet
#define __FFI_IGNORE__

#include "ArrayList.h"
#include "Common.h"
#include "Draw.h"
#include "Matrix.h"
#include "MatrixDef.h"
#include "Physics.h"
#include "Quat.h"
#include "Ray.h"
#include "RigidBody.h"
#include "Trigger.h"
#include "Vec3.h"
#include "Vec4.h"

#pragma warning(push)
#pragma warning(disable : 4127) // conditional is constant
#pragma warning(disable : 4577) // noexcept with no mode specified
#pragma warning(disable : 4625) // deleted copy constructor
#pragma warning(disable : 4626) // deleted assignemnt operator
#pragma warning(disable : 5027) // deleted move operator
#include "bullet/btBulletDynamicsCommon.h"
#include "bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#pragma warning(pop)

inline static Vec3f Vec3f_FromBullet (btVector3 const& v) {
  return Vec3f_Create(v.getX(), v.getY(), v.getZ());
}

inline static btVector3 Vec3f_ToBullet (Vec3f const* v) {
  return btVector3(UNPACK3(*v));
}

inline static Quat Quat_FromBullet (btQuaternion const& q) {
  return Quat_Create(q.getX(), q.getY(), q.getZ(), q.getW());
}

inline static btQuaternion Quat_ToBullet (Quat const* q) {
  return btQuaternion(UNPACK4(*q));
}

inline static void Matrix_FromTransform (btTransform* transform, Matrix* mat, float scale) {
  float* m = mat->m;
  transform->getOpenGLMatrix(m);

  /* NOTE : Inject scale. */
  m[0] *= scale; m[1] *= scale; m[ 2] *= scale;
  m[4] *= scale; m[5] *= scale; m[ 6] *= scale;
  m[8] *= scale; m[9] *= scale; m[10] *= scale;

  /* NOTE : Fix storage order. */
  Matrix_ITranspose(mat);
}

struct DebugDrawer : btIDebugDraw {
  int debugMode;
  virtual void setDebugMode(int _debugMode) { debugMode = _debugMode; }
  virtual int  getDebugMode() const { return debugMode; }

  virtual void reportErrorWarning(const char* warningString) {
    Warn(warningString);
  }

  virtual void drawLine(const btVector3& _from, const btVector3& _to, const btVector3&) {
    Vec3f from = Vec3f_FromBullet(_from);
    Vec3f to   = Vec3f_FromBullet(_to);
    Draw_Line3(&from, &to);
  }

  virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3&) {
    drawLine(from, to, fromColor);
  }

  virtual void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&) { NYI; }
  virtual void draw3dText(const btVector3&, const char*) { NYI; }
};

struct RayCastCallback : public btCollisionWorld::RayResultCallback {
  btBroadphaseProxy* selfProxy;
  Ray*               ray;
  RayCastResult*     result;

  virtual bool needsCollision(btBroadphaseProxy* proxy0) const {
    bool collides = true;
    collides = collides && (proxy0->m_collisionFilterGroup & m_collisionFilterMask) != 0;
    collides = collides && (m_collisionFilterGroup & proxy0->m_collisionFilterMask) != 0;
    collides = collides && (!selfProxy || proxy0 != selfProxy);
    return collides;
  }

  virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) {
    /* OPTIMIZE : If performance becomes an issue, most of this can be deferred. */

    m_closestHitFraction = rayResult.m_hitFraction;
    m_collisionObject    = rayResult.m_collisionObject;

    RigidBody* body = (RigidBody*) rayResult.m_collisionObject->getUserPointer();
    if (rayResult.m_localShapeInfo) {
      int iCompound = rayResult.m_localShapeInfo->m_triangleIndex;
      body = RigidBody_GetPart(body, iCompound);
    }
    result->body = body;

    btVector3 normal = rayResult.m_hitNormalLocal;
    if (!normalInWorldSpace)
      normal = m_collisionObject->getWorldTransform().getBasis() * normal;
    result->norm = Vec3f_FromBullet(normal);

    result->t = Lerp(ray->tMin, ray->tMax, rayResult.m_hitFraction);
    Ray_GetPoint(ray, result->t, &result->pos);

    return m_closestHitFraction;
  }
};

struct ShapeCastCallback : public btCollisionWorld::ContactResultCallback {
  btCollisionObject* castShape;
  btBroadphaseProxy* selfProxy;
  ShapeCastResult*   result;

  virtual bool needsCollision(btBroadphaseProxy* proxy0) const {
    bool collides = true;
    collides = collides && (proxy0->m_collisionFilterGroup & m_collisionFilterMask) != 0;
    collides = collides && (m_collisionFilterGroup & proxy0->m_collisionFilterMask) != 0;
    collides = collides && (!selfProxy || proxy0 != selfProxy);
    return collides;
  }

  virtual btScalar addSingleResult(
    btManifoldPoint&                cp,
    const btCollisionObjectWrapper* colObj0Wrap,
    int                             partId0,
    int                             index0,
    const btCollisionObjectWrapper* colObj1Wrap,
    int                             partId1,
    int                             index1) {
    /* OPTIMIZE : Assuming multiple contacts for the same object will be
                  sequential, we can remember the last object and short circuit
                  rather than searching the result list for duplicates. */

    /* WARNING : partId# and index# are uninitialized when the object is not a compound! */

    UNUSED(cp); UNUSED(partId0); UNUSED(index0); UNUSED(partId1); UNUSED(index1);
    Assert(colObj0Wrap->m_collisionObject == castShape
        || colObj1Wrap->m_collisionObject == castShape);

    colObj0Wrap = colObj0Wrap->m_collisionObject == castShape ? colObj1Wrap : colObj0Wrap;

    RigidBody* body = (RigidBody*) colObj0Wrap->m_collisionObject->getUserPointer();
    if (colObj0Wrap->m_index != -1)
      body = RigidBody_GetPart(body, colObj0Wrap->m_index);

    bool duplicate = false;
    for (int i = 0; i < ArrayList_GetSize(result->hits); i++) {
      duplicate |= ArrayList_Get(result->hits, i) == body;
    }

    if (!duplicate)
      ArrayList_Append(result->hits, body);

     /* NOTE : Unused by Bullet. */
     return 1;
  }
};

struct ShapeOverlapCallback : public btCollisionWorld::ContactResultCallback {
  btCollisionObject* castShape;
  btBroadphaseProxy* selfProxy;
  bool               foundHit;

  virtual bool needsCollision(btBroadphaseProxy* proxy0) const {
    bool collides = true;
    collides = collides && (proxy0->m_collisionFilterGroup & m_collisionFilterMask) != 0;
    collides = collides && (m_collisionFilterGroup & proxy0->m_collisionFilterMask) != 0;
    collides = collides && (!selfProxy || proxy0 != selfProxy);
    collides = collides && !foundHit;
    return collides;
  }

  virtual btScalar addSingleResult(
    btManifoldPoint&                cp,
    const btCollisionObjectWrapper* colObj0Wrap,
    int                             partId0,
    int                             index0,
    const btCollisionObjectWrapper* colObj1Wrap,
    int                             partId1,
    int                             index1) {
    UNUSED(cp);
    UNUSED(colObj0Wrap); UNUSED(partId0); UNUSED(index0);
    UNUSED(colObj1Wrap); UNUSED(partId1); UNUSED(index1);
    Assert(colObj0Wrap->m_collisionObject == castShape
        || colObj1Wrap->m_collisionObject == castShape);

    foundHit = true;

     /* NOTE : Unused by Bullet. */
     return 1;
  }
};

struct GhostObject : public btGhostObject {
  /* HACK : Ensure checkCollideWithOverride will get called. */
  GhostObject() {
    setIgnoreCollisionCheck(0, true);
  }

  /* NOTE: This prevents Triggers from tracking other triggers and the RigidBody
           they are attached to. Anything that gets past this filter will be in
           Trigger_GetContents. */
  bool needBroadphaseCollision(const btCollisionObject* co) const {
    typedef btCollisionObject::CollisionObjectTypes Type;
    bool collide = true;
    collide = collide && co->getInternalType() != Type::CO_GHOST_OBJECT;
    collide = collide && co->getUserPointer() != Trigger_GetParent((Trigger*) getUserPointer());
    return collide;
  }

  /* NOTE: Triggers *never* want narrowphase collision. This prevents wasted
           work calculating contact points between triggers and the objects
           within them. Since tracking happens in broadphase we've already add
           the object to our list. */
  virtual bool checkCollideWithOverride(const btCollisionObject*) const {
     return false;
  }
};

struct BroadphaseFilterCallback : public btOverlapFilterCallback {
  virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const {
    btCollisionObject* colObj0 = (btCollisionObject*) proxy0->m_clientObject;
    btCollisionObject* colObj1 = (btCollisionObject*) proxy1->m_clientObject;

    typedef btCollisionObject::CollisionObjectTypes Type;
    bool collide = true;
    if (colObj0->getInternalType() == Type::CO_GHOST_OBJECT)
      collide = collide && ((GhostObject*) colObj0)->needBroadphaseCollision(colObj1);
    if (colObj1->getInternalType() == Type::CO_GHOST_OBJECT)
      collide = collide && ((GhostObject*) colObj1)->needBroadphaseCollision(colObj0);
    return collide;
  }
};

#endif

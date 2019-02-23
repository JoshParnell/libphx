#include "Bullet.h"
#include "CollisionShape.h"
#include "PhxMath.h"
#include "Physics.h"
#include "PhysicsDefs.h"
#include "Quat.h"
#include "Ray.h"
#include "RenderState.h"
#include "RigidBody.h"
#include "Shader.h"
#include "Sphere.h"
#include "Trigger.h"

struct Physics {
  btDefaultCollisionConfiguration*     collisionConfiguration;
  btCollisionDispatcher*               dispatcher;
  btBroadphaseInterface*               broadphase;
  btSequentialImpulseConstraintSolver* solver;
  btDiscreteDynamicsWorld*             dynamicsWorld;
  DebugDrawer*                         debugDrawer;
  btGhostPairCallback*                 ghostCallback;
  BroadphaseFilterCallback*            broadphaseFilterCallback;
  ArrayList(Trigger*,                  triggers);
};

Physics* Physics_Create () {
  Physics* self = MemNewZero(Physics);

  /* NOTE : btSequentialImpulseConstraintSolver is Projected Gauss Seidel
            btDbvtBroadphase is dynamic bounding volume tree */
  self->collisionConfiguration   = new btDefaultCollisionConfiguration();
  self->dispatcher               = new btCollisionDispatcher(self->collisionConfiguration);
  self->broadphase               = new btDbvtBroadphase();
  self->solver                   = new btSequentialImpulseConstraintSolver();
  self->dynamicsWorld            = new btDiscreteDynamicsWorld(self->dispatcher, self->broadphase, self->solver, self->collisionConfiguration);
  self->debugDrawer              = new DebugDrawer();
  self->ghostCallback            = new btGhostPairCallback();
  self->broadphaseFilterCallback = new BroadphaseFilterCallback();
  ArrayList_Reserve(self->triggers, 32);

  self->dynamicsWorld->setDebugDrawer(self->debugDrawer);
  self->dynamicsWorld->setGravity(btVector3(0, 0, 0));
  self->dynamicsWorld->setWorldUserInfo(self);

  btContactSolverInfo& solverInfo = self->dynamicsWorld->getSolverInfo();
  solverInfo.m_numIterations = 4;

  btOverlappingPairCache* pairCache = self->broadphase->getOverlappingPairCache();
  pairCache->setInternalGhostPairCallback(self->ghostCallback);
  pairCache->setOverlapFilterCallback(self->broadphaseFilterCallback);

  return self;
}

void Physics_Free (Physics* self) {
  /* NOTE: Collision objects have a world index and broadphase proxy that need
           to be cleared so actually have to remove them all. */

  btCollisionObjectArray& collisionObjects = self->dynamicsWorld->getCollisionObjectArray();
  for (int i = collisionObjects.size() - 1; i >= 0; i--) {
    btCollisionObject* collisionObject = collisionObjects[i];
    void*              userPointer     = collisionObject->getUserPointer();

    PhysicsType type = *((PhysicsType*) userPointer);
    switch (type) {
      case PhysicsType_RigidBody: {
        RigidBody* rigidBody = (RigidBody*) userPointer;
        rigidBody->physics = 0;
        self->dynamicsWorld->removeRigidBody(rigidBody->handle);
        break;
      }

      case PhysicsType_Trigger: {
        Trigger* trigger = (Trigger*) userPointer;
        trigger->physics = 0;
        self->dynamicsWorld->removeCollisionObject(trigger->handle);
        break;
      }
    }
  }

  ArrayList_Free(self->triggers);
  delete self->broadphaseFilterCallback;
  delete self->ghostCallback;
  delete self->debugDrawer;
  delete self->dynamicsWorld;
  delete self->solver;
  delete self->broadphase;
  delete self->dispatcher;
  delete self->collisionConfiguration;
  MemFree(self);
}

void Physics_AddRigidBody (Physics* self, RigidBody* rigidBody) {
  /* TODO: I'd rather fatal here, but RigidBody.lua adds indiscriminately. */
  if (RigidBody_IsChild(rigidBody)) return;

  if (rigidBody->physics)
    Fatal("Physics_Add: Object is already added to physics.");

  typedef btCollisionObject::CollisionFlags Flags;
  int flags = rigidBody->handle->getCollisionFlags();
  bool collidable = !HAS_FLAG(flags, Flags::CF_NO_CONTACT_RESPONSE);
  int group = collidable ? rigidBody->collisionGroup : 0;

  rigidBody->physics = self;
  self->dynamicsWorld->addRigidBody(rigidBody->handle, group, rigidBody->collisionMask);

  RigidBody* child = rigidBody;
  while (child) {
    Trigger* trigger = child->triggers;
    while (trigger) {
      Physics_AddTrigger(self, trigger);
      trigger = trigger->next;
    }
    child = child->next;
  }
}

void Physics_RemoveRigidBody (Physics* self, RigidBody* rigidBody) {
  /* TODO: I'd rather fatal here, but RigidBody.lua removes indiscriminately. */
  if (RigidBody_IsChild(rigidBody)) return;

  if (!rigidBody->physics)
    Fatal("Physics_Remove: Object is not added to physics.");
  if (rigidBody->physics != self)
    Fatal("Physics_Remove: Object is not added to this physics world.");

  rigidBody->physics = 0;
  self->dynamicsWorld->removeRigidBody(rigidBody->handle);

  RigidBody* child = rigidBody;
  while (child) {
    Trigger* trigger = child->triggers;
    while (trigger) {
      Physics_RemoveTrigger(self, trigger);
      trigger = trigger->next;
    }
    child = child->next;
  }
}

void Physics_AddTrigger (Physics* self, Trigger* trigger) {
  if (trigger->physics)
    Fatal("Physics_AddTrigger: Object is already added to physics.");

  ArrayList_Append(self->triggers, trigger);
  trigger->physics = self;
  self->dynamicsWorld->addCollisionObject(trigger->handle, trigger->collisionGroup, trigger->collisionMask);
}

void Physics_RemoveTrigger (Physics* self, Trigger* trigger) {
  if (!trigger->physics)
    Fatal("Physics_RemoveTrigger: Object is not added to physics.");
  if (trigger->physics != self)
    Fatal("Physics_RemoveTrigger: Object is not added to this physics world.");

  ArrayList_RemoveFast(self->triggers, trigger);
  trigger->physics = 0;
  self->dynamicsWorld->removeCollisionObject(trigger->handle);
}

bool Physics_GetNextCollision (Physics* self, Collision* it) {
  int collisionCount = self->dispatcher->getNumManifolds();
  while (it->index < collisionCount) {
    btPersistentManifold* manifold = self->dispatcher->getManifoldByIndexInternal(it->index++);

    if (manifold->getNumContacts() > 0) {
      btManifoldPoint point = manifold->getContactPoint(0);
      it->count++;
      it->body0 = (RigidBody*) manifold->getBody0()->getUserPointer();
      it->body1 = (RigidBody*) manifold->getBody1()->getUserPointer();
      if (RigidBody_IsCompound(it->body0)) it->body0 = RigidBody_GetPart(it->body0, point.m_index0);
      if (RigidBody_IsCompound(it->body1)) it->body1 = RigidBody_GetPart(it->body1, point.m_index1);
      return true;
    }
  }
  it->body0 = 0;
  it->body1 = 0;
  return false;
}

void Physics_Update (Physics* self, float dt) {
  for (int i = 0; i < ArrayList_GetSize(self->triggers); i++) {
    Trigger* trigger = ArrayList_Get(self->triggers, i);
    Trigger_Update(trigger);
  }
  self->dynamicsWorld->stepSimulation(dt, 0);
}

void Physics_RayCast (Physics* self, Ray* ray, RayCastResult* result) {
  typedef btTriangleRaycastCallback::EFlags Flags;

  Vec3f from; Ray_GetPoint(ray, ray->tMin, &from);
  Vec3f to;   Ray_GetPoint(ray, ray->tMax, &to);
  btVector3 btFrom = Vec3f_ToBullet(&from);
  btVector3 btTo   = Vec3f_ToBullet(&to);

  *result = {};
  RayCastCallback callback = {};
  callback.ray                    = ray;
  callback.result                 = result;
  callback.m_flags                = Flags::kF_FilterBackfaces | Flags::kF_UseSubSimplexConvexCastRaytest;
  callback.m_collisionFilterGroup = CollisionGroup_Default;
  callback.m_collisionFilterMask  = CollisionMask_All;
  self->dynamicsWorld->rayTest(btFrom, btTo, callback);
}

inline static void Physics_ShapeCast (Physics* self, btConvexShape* shape, Vec3f* pos, Quat* rot, ShapeCastResult* result) {
  btTransform btPos = btTransform(Quat_ToBullet(rot), Vec3f_ToBullet(pos));
  *result = {};

  btCollisionObject* proxy = new btCollisionObject();
  proxy->setCollisionShape(shape);
  proxy->setWorldTransform(btPos);

  ShapeCastCallback callback = {};
  callback.castShape              = proxy;
  callback.result                 = result;
  callback.m_collisionFilterGroup = CollisionGroup_Default;
  callback.m_collisionFilterMask  = CollisionMask_All;
  self->dynamicsWorld->contactTest(proxy, callback);
}

void Physics_SphereCast (Physics* self, Sphere* sphere, ShapeCastResult* result) {
  btSphereShape btSphere = btSphereShape(sphere->r);
  Quat rot; Quat_Identity(&rot);
  Physics_ShapeCast(self, &btSphere, &sphere->p, &rot, result);
}

void Physics_BoxCast (Physics* self, Vec3f* pos, Quat* rot, Vec3f* halfExtents, ShapeCastResult* result) {
  btBoxShape btBox = btBoxShape(Vec3f_ToBullet(halfExtents));
  Physics_ShapeCast(self, &btBox, pos, rot, result);
}

inline static bool Physics_ShapeOverlap (Physics* self, btConvexShape* shape, Vec3f* pos, Quat* rot) {
  btTransform btPos = btTransform(Quat_ToBullet(rot), Vec3f_ToBullet(pos));

  btCollisionObject* proxy = new btCollisionObject();
  proxy->setCollisionShape(shape);
  proxy->setWorldTransform(btPos);

  ShapeOverlapCallback callback = {};
  callback.castShape              = proxy;
  callback.m_collisionFilterGroup = CollisionGroup_Default;
  callback.m_collisionFilterMask  = CollisionMask_All;
  self->dynamicsWorld->contactTest(proxy, callback);
  return callback.foundHit;
}

bool Physics_SphereOverlap (Physics* self, Sphere* sphere) {
  btSphereShape btSphere = btSphereShape(sphere->r);
  Quat rot; Quat_Identity(&rot);
  return Physics_ShapeOverlap(self, &btSphere, &sphere->p, &rot);
}

bool Physics_BoxOverlap (Physics* self, Vec3f* pos, Quat* rot, Vec3f* halfExtents) {
  btBoxShape btBox = btBoxShape(Vec3f_ToBullet(halfExtents));
  return Physics_ShapeOverlap(self, &btBox, pos, rot);
}

void Physics_PrintProfiling (Physics* self) {
  if (!self->dynamicsWorld) return;

  int numActive = 0;
  btCollisionObjectArray& collisionObjects = self->dynamicsWorld->getCollisionObjectArray();
  for (int i = 0; i < collisionObjects.size(); i++) {
    btCollisionObject* collisionObject = collisionObjects[i];
    if (collisionObject->isActive())
      numActive++;
  }
  Warn("Active %i / Total %i", numActive, collisionObjects.size());

  CProfileManager::dumpAll();
}

void Physics_DrawBoundingBoxesLocal (Physics* self) {
  RenderState_PushWireframe(true);
  btCollisionObjectArray& collisionObjects = self->dynamicsWorld->getCollisionObjectArray();
  for (int i = 0; i < collisionObjects.size(); i++) {
    btCollisionObject* collisionObject = collisionObjects[i];
    void*              userPointer     = collisionObject->getUserPointer();

    PhysicsType type = *((PhysicsType*) userPointer);
    if (type == PhysicsType_RigidBody) {
      RigidBody* rigidBody = (RigidBody*) userPointer;

      if (RigidBody_IsParent(rigidBody)) {
        Box3f box; RigidBody_GetBoundingBoxLocalCompound(rigidBody, &box);
        Vec3f pos; RigidBody_GetPos(rigidBody, &pos);
        Quat  rot; RigidBody_GetRot(rigidBody, &rot);
        Matrix* mat = Matrix_FromPosRot(&pos, &rot);
        Shader_SetMatrix("mWorld", mat);
        Draw_Box3(&box);
        Matrix_Free(mat);
      }

      while (rigidBody) {
        Box3f box; RigidBody_GetBoundingBoxLocal(rigidBody, &box);
        Vec3f pos; RigidBody_GetPos(rigidBody, &pos);
        Quat  rot; RigidBody_GetRot(rigidBody, &rot);
        Matrix* mat = Matrix_FromPosRot(&pos, &rot);
        Shader_SetMatrix("mWorld", mat);
        Draw_Box3(&box);
        Matrix_Free(mat);
        rigidBody = rigidBody->next;
      }
    }
  }
  RenderState_PopWireframe();
}

void Physics_DrawBoundingBoxesWorld (Physics* self) {
  RenderState_PushWireframe(true);
  btCollisionObjectArray& collisionObjects = self->dynamicsWorld->getCollisionObjectArray();
  for (int i = 0; i < collisionObjects.size(); i++) {
    btCollisionObject* collisionObject = collisionObjects[i];
    void*              userPointer     = collisionObject->getUserPointer();

    PhysicsType type = *((PhysicsType*) userPointer);
    if (type == PhysicsType_RigidBody) {
      RigidBody* rigidBody = (RigidBody*) userPointer;

      if (RigidBody_IsParent(rigidBody)) {
        Box3f box; RigidBody_GetBoundingBoxCompound(rigidBody, &box);
        Draw_Box3(&box);
      }

      while (rigidBody) {
        Box3f box; RigidBody_GetBoundingBox(rigidBody, &box);
        Draw_Box3(&box);
        rigidBody = rigidBody->next;
      }
    }
  }
  RenderState_PopWireframe();
}

void Physics_DrawTriggers (Physics* self) {
  RenderState_PushWireframe(true);
  btCollisionObjectArray& collisionObjects = self->dynamicsWorld->getCollisionObjectArray();
  for (int i = 0; i < collisionObjects.size(); i++) {
    btCollisionObject* collisionObject = collisionObjects[i];
    void*              userPointer     = collisionObject->getUserPointer();

    PhysicsType type = *((PhysicsType*) userPointer);
    if (type == PhysicsType_Trigger) {
      Trigger* trigger = (Trigger*) userPointer;

      Box3f box; Trigger_GetBoundingBox(trigger, &box);
      Draw_Box3(&box);
    }
  }
  RenderState_PopWireframe();
}

void Physics_DrawWireframes (Physics* self) {
  self->debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

  btCollisionObjectArray& collisionObjects = self->dynamicsWorld->getCollisionObjectArray();
  for (int i = 0; i < collisionObjects.size(); i++) {
    btCollisionObject* collisionObject = collisionObjects[i];
    void*              userPointer     = collisionObject->getUserPointer();

    PhysicsType type = *((PhysicsType*) userPointer);
    if (type == PhysicsType_RigidBody) {
      RigidBody* rigidBody = (RigidBody*) userPointer;

      while (rigidBody) {
        /* TODO: Do better. */
        btTransform RigidBody_GetWorldTransform (RigidBody*);

        btTransform       transform = RigidBody_GetWorldTransform(rigidBody);
        CollisionShape*   shape     = CollisionShape_GetCached(rigidBody->iShape);
        if (shape->type == CollisionShapeType_Hull) {
          /* HACK: Drawing a btUniformScalingShape does nothing. The underlying
                   hull has a scale of 1. So we draw the hull directly and hack
                   the scale into the transform. Inefficiently because scaled
                   creates a copy. */
          btVector3 scale = btVector3(REPEAT3(shape->scale));
          transform.setBasis(transform.getBasis().scaled(scale));
          self->dynamicsWorld->debugDrawObject(transform, shape->hull.hullHandle, btVector3());
        } else {
          self->dynamicsWorld->debugDrawObject(transform, shape->base.handle, btVector3());
        }

        rigidBody = rigidBody->next;
      }
    }
  }
}

void Physics_FlushCachedRigidBodyData (Physics* self, RigidBody* rigidBody) {
  /* NOTE: Broadphase implementations have a 'pair cache' internally. Each pair
           contains data for the two objects that are touching, plus an
           indicator for which collision algorithm should be used. The algorithm
           chosen depends on the shape of each object. If a new pair is added
           and there is an existing pair for the same objects (I'm not sure if
           one or both objects need to match) the algorithm will be copied from
           that, instead of determined from scratch. Further, the algorithm is
           cached for the duration of the contact (I think). Thus, if the shape
           of an object changes while it is part of any pairs Bullet may apply
           the wrong algorithm. This leads to crashes and other bad behavior
           when Bullet later casts the btCollisionShape* stored in the pair to a
           specific shape expected by the algorithm and tries to use it. */

  btBroadphaseProxy*      bpProxy   = rigidBody->handle->getBroadphaseHandle();
  btOverlappingPairCache* pairCache = self->broadphase->getOverlappingPairCache();
  pairCache->cleanProxyFromPairs(bpProxy, self->dispatcher);
}

#include "Bullet.h"
#include "CollisionShape.h"
#include "PhxMemory.h"
#include "Physics.h"
#include "PhysicsDefs.h"
#include "RigidBody.h"
#include "Trigger.h"

/* --- Helper Functions ----------------------------------------------------- */

bool RigidBody_IsChild (RigidBody* self) {
  return self->parent != 0 && self->parent != self;
}

bool RigidBody_IsCompound (RigidBody* self) {
  return self->parent != 0;
}

bool RigidBody_IsParent (RigidBody* self) {
  return self->parent != 0 && self->parent == self;
}

inline static void RigidBody_SetFlag (RigidBody* self, int flag, bool enable) {
  btRigidBody* rigidBody = self->handle;

  int flags = rigidBody->getCollisionFlags();
  flags = enable ? flags | flag : flags & ~flag;
  rigidBody->setCollisionFlags(flags);
}

RigidBody* RigidBody_GetPart (RigidBody* self, int iCompound) {
  Assert(RigidBody_IsCompound(self));
  self = self->parent;
  while (self->iCompound != iCompound) self = self->next;
  return self;
}

btTransform RigidBody_GetWorldTransform (RigidBody* self) {
  btTransform transform;

  if (!RigidBody_IsChild(self)) {
    btRigidBody* rigidBody = self->handle;
    transform = rigidBody->getWorldTransform();
  } else {
    btRigidBody*    rigidBody = self->parent->handle;
    CollisionShape* cmpShape  = CollisionShape_GetCached(self->iCompoundShape);
    transform = rigidBody->getWorldTransform();
    transform *= cmpShape->compound.handle->getChildTransform(self->iCompound);
  }

  return transform;
}

inline static void RigidBody_RecalculateInertia (RigidBody* self) {
  /* NOTE : We use the parent's shape to calculate inertia. Compound shapes
            calculate inertia from the bounding box, not the actual shapes, so
            if we e.g. attach something to a ship using a sphere collision shape
            all of a sudden it will switch to inertia for a box and drastically
            change the controls. It looks like the proper solution would be to
            use btCompoundShape::calculatePrincipalAxisTransform to get the
            proper inertia and offset the center of mass. In turn, this means we
            have to get rid of our assumption that the parent has identity
            position within the compound. However, this seems unlikely to add
            much value to the game, so we aren't going to bother. */

  btRigidBody*    rigidBody = self->handle;
  CollisionShape* shape     = CollisionShape_GetCached(self->iShape);

  btVector3 inertia(0, 0, 0);
  shape->base.handle->calculateLocalInertia(self->mass, inertia);
  /* HACK: Hulls are wrapped in btUniformScalingShape. When calculating inertia,
           first it's calculated for a unit sized version of the underlying
           shape, then the scaling wrapper multiplies is by the scale. However,
           inertia is rougly proportional to r^2 and this means the final
           calculation is off by an additional factor of scale. We fix this here
           to ensure wrapped hull has approximately the same inertia a plain
           scaled hull would. Otherwise inertia is flat out wrong and the
           presence of a btUniformScalingShape wrapper leads to wildly different
           ship controls for the same model. */
  if (shape->type == CollisionShapeType_Hull)
    inertia *= shape->hull.handle->getUniformScalingFactor();

  rigidBody->setMassProps(self->mass, inertia);
  rigidBody->updateInertiaTensor();
}

inline static RigidBody* RigidBody_Create (CollisionShape* shape) {
  /* NOTE : We only create compounds through attaching. */
  Assert(shape->type != CollisionShapeType_Compound);

  RigidBody* self = MemNewZero(RigidBody);
  self->type           = PhysicsType_RigidBody;
  self->mass           = 1.0f;
  self->iCompound      = -1;
  self->iCompoundShape = -1;

  btVector3 inertia(0, 0, 0);
  shape->base.handle->calculateLocalInertia(self->mass, inertia);

  btRigidBody* rigidBody = new btRigidBody(self->mass, 0, shape->base.handle, inertia);
  rigidBody->setFlags(BT_DISABLE_WORLD_GRAVITY);
  rigidBody->setRestitution(0.4f);
  rigidBody->setUserPointer(self);

  self->handle         = rigidBody;
  self->iShape         = shape->iShape;
  self->collisionGroup = CollisionGroup_Default;
  self->collisionMask  = CollisionMask_All;
  return self;
}

/* --- Implementation ------------------------------------------------------- */

RigidBody* RigidBody_CreateBox () {
  Vec3f halfExtents = { 1, 1, 1 };
  CollisionShape* shape = CollisionShape_CreateBox(&halfExtents);
  return RigidBody_Create(shape);
}

RigidBody* RigidBody_CreateBoxFromMesh (Mesh* mesh) {
  CollisionShape* shape = CollisionShape_CreateBoxFromMesh(mesh);
  return RigidBody_Create(shape);
}

RigidBody* RigidBody_CreateSphere () {
  CollisionShape* shape = CollisionShape_CreateSphere(1);
  return RigidBody_Create(shape);
}

RigidBody* RigidBody_CreateSphereFromMesh (Mesh* mesh) {
  CollisionShape* shape = CollisionShape_CreateSphereFromMesh(mesh);
  return RigidBody_Create(shape);
}

RigidBody* RigidBody_CreateHullFromMesh (Mesh* mesh) {
  CollisionShape* shape = CollisionShape_CreateHullFromMesh(mesh);
  return RigidBody_Create(shape);
}

inline static void RigidBody_FreeImpl (RigidBody* self) {
  Trigger* trigger = self->triggers;
  while (trigger) {
    Trigger* toFree = trigger;
    trigger = trigger->next;
    Trigger_Free(toFree);
  }

  CollisionShape* shape = CollisionShape_GetCached(self->iShape);
  CollisionShape_Free(shape);
  delete self->handle;
  MemFree(self);
}

void RigidBody_Free (RigidBody* self) {
  if (RigidBody_IsChild(self))
    Fatal("RigidBody_Free: Not supported on children.");
  if (self->physics)
    Fatal("RigidBody_Free: Object is still present in Physics.");

  if (RigidBody_IsParent(self)) {
    RigidBody* child = self->next;
    while (child) {
      RigidBody* toFree = child;
      child = child->next;
      RigidBody_FreeImpl(toFree);
    }

    CollisionShape* shape = CollisionShape_GetCached(self->iCompoundShape);
    CollisionShape_Free(shape);
  }

  RigidBody_FreeImpl(self);
}

void RigidBody_ApplyForce (RigidBody* self, Vec3f* force) {
  if (RigidBody_IsChild(self))
    Fatal("RigidBody_ApplyForce: Not supported on children.");

  btRigidBody* rigidBody = self->handle;
  rigidBody->applyCentralForce(Vec3f_ToBullet(force));
  rigidBody->activate();
}

void RigidBody_ApplyTorque (RigidBody* self, Vec3f* torque) {
  if (RigidBody_IsChild(self))
    Fatal("RigidBody_ApplyTorque: Not supported on children.");

  btRigidBody* rigidBody = self->handle;
  rigidBody->applyTorque(Vec3f_ToBullet(torque));
  rigidBody->activate();
}

void RigidBody_Attach (RigidBody* parent, RigidBody* child, Vec3f* pos, Quat* rot) {
  if (RigidBody_IsChild(parent))
    Fatal("RigidBody_Attach: Recursive attachment is not supported. Parent is already attached to something.");
  if (RigidBody_IsCompound(child))
    Fatal("RigidBody_Attach: Child is already part of a compound.");
  if (parent == child)
    Fatal("RigidBody_Attach: Cannot attach object to itself.");
  if (child->physics)
    Fatal("RigidBody_Attach: Child has not been removed from physics.");

  //Convert parent to a compound
  if (!RigidBody_IsParent(parent)) {
    btRigidBody*    pBody  = parent->handle;
    CollisionShape* pShape = CollisionShape_GetCached(parent->iShape);
    btTransform     pTrans = btTransform::getIdentity();

    CollisionShape shapeDef = {};
    shapeDef.scale = 1.0f;
    shapeDef.type  = CollisionShapeType_Compound;
    CollisionShape* cmpShape = CollisionShape_Create(shapeDef);

    cmpShape->compound.handle->addChildShape(pTrans, pShape->base.handle);
    pBody->setCollisionShape(cmpShape->compound.handle);
    parent->iCompound      = 0;
    parent->iCompoundShape = cmpShape->iShape;
    parent->parent         = parent;
    Physics_FlushCachedRigidBodyData(parent->physics, parent);
  }

  /* NOTE : Position is relative to the unscaled parent. */
  CollisionShape* pShape   = CollisionShape_GetCached(parent->iShape);
  CollisionShape* cmpShape = CollisionShape_GetCached(parent->iCompoundShape);
  CollisionShape* cShape   = CollisionShape_GetCached(child->iShape);
  btTransform     cTrans   = btTransform(Quat_ToBullet(rot), Vec3f_ToBullet(pos) * pShape->scale);

  //Insert child into the compound list
  child->parent = parent;
  child->next   = parent->next;
  parent->next  = child;

  //Add child to the compound
  cmpShape->compound.handle->addChildShape(cTrans, cShape->base.handle);
  child->iCompound      = cmpShape->compound.handle->getNumChildShapes() - 1;
  child->iCompoundShape = cmpShape->iShape;
}

void RigidBody_Detach (RigidBody* parent, RigidBody* child) {
  if (child->parent != parent)
    Fatal("RigidBody_Detach: Child is not attached to parent.");
  if (child == parent)
    Fatal("RigidBody_Detach: Cannot detach object from itself.");

  btRigidBody*     pBody       = parent->handle;
  btRigidBody*     cBody       = child->handle;
  btCompoundShape* compound    = (btCompoundShape*) pBody->getCollisionShape();
  btTransform      cLocalTrans = compound->getChildTransform(child->iCompound);

  //Remove child from the compound
  { /* HACK : btCompoundShape does a 'remove fast' internally. */
    int iLast = compound->getNumChildShapes() - 1;
    RigidBody* last = parent;
    while (last->iCompound != iLast) last = last->next;
    last->iCompound = child->iCompound;
  }
  compound->removeChildShapeByIndex(child->iCompound);
  child->iCompound      = -1;
  child->iCompoundShape = -1;

  //Remove child from the compound list
  RigidBody* prev = parent;
  while (prev->next != child) prev = prev->next;
  prev->next    = child->next;
  child->next   = 0;
  child->parent = 0;

  //Apply current position, rotation, and velocity
  btTransform  cTrans = pBody->getWorldTransform() * cLocalTrans;
  btVector3    cVel   = pBody->getVelocityInLocalPoint(cLocalTrans.getOrigin());
  cBody->setWorldTransform(cTrans);
  cBody->setLinearVelocity(cVel);

  //Convert parent to single object
  if (parent->next == 0) {
    CollisionShape* pShape = CollisionShape_GetCached(parent->iShape);

    compound->removeChildShapeByIndex(parent->iCompound);
    pBody->setCollisionShape(pShape->base.handle);
    parent->iCompound      = -1;
    parent->iCompoundShape = -1;
    parent->parent         = 0;
    Physics_FlushCachedRigidBodyData(parent->physics, parent);
  } else {
    compound->recalculateLocalAabb();
  }
}

/* TODO: Should scale be inverted? */
Matrix* RigidBody_GetToLocalMatrix (RigidBody* self) {
  float       scale     = RigidBody_GetScale(self);
  btTransform transform = RigidBody_GetWorldTransform(self).inverse();
  Matrix_FromTransform(&transform, &self->mat, scale);
  return &self->mat;
}

Matrix* RigidBody_GetToWorldMatrix (RigidBody* self) {
  float       scale     = RigidBody_GetScale(self);
  btTransform transform = RigidBody_GetWorldTransform(self);
  Matrix_FromTransform(&transform, &self->mat, scale);
  return &self->mat;
}

void RigidBody_SetCollidable (RigidBody* self, bool collidable) {
  typedef btCollisionObject::CollisionFlags Flags;
  RigidBody_SetFlag(self, Flags::CF_NO_CONTACT_RESPONSE, !collidable);

  if (self->physics) {
    btRigidBody*       rigidBody = self->handle;
    btBroadphaseProxy* proxy     = rigidBody->getBroadphaseHandle();

    proxy->m_collisionFilterGroup = collidable ? self->collisionGroup : 0;
  }
}

void RigidBody_SetCollisionGroup (RigidBody* self, int group) {
  btRigidBody* rigidBody = self->handle;
  self->collisionGroup = group;

  if (self->physics) {
    typedef btCollisionObject::CollisionFlags Flags;
    btBroadphaseProxy* proxy = rigidBody->getBroadphaseHandle();

    int  flags      = rigidBody->getCollisionFlags();
    bool collidable = !HAS_FLAG(flags, Flags::CF_NO_CONTACT_RESPONSE);
    proxy->m_collisionFilterGroup = collidable ? self->collisionGroup : 0;
  }
}

void RigidBody_SetCollisionMask (RigidBody* self, int mask) {
  btRigidBody* rigidBody = self->handle;
  self->collisionMask = mask;

  if (self->physics) {
    btBroadphaseProxy* proxy = rigidBody->getBroadphaseHandle();
    proxy->m_collisionFilterMask = mask;
  }
}

void RigidBody_SetDrag (RigidBody* self, float linear, float angular) {
  btRigidBody* rigidBody = self->handle;
  rigidBody->setDamping(linear, angular);
}

void RigidBody_SetFriction (RigidBody* self, float friction) {
  btRigidBody* rigidBody = self->handle;
  rigidBody->setFriction(friction);
}

void RigidBody_SetKinematic (RigidBody* self, bool kinematic) {
  typedef btCollisionObject::CollisionFlags Flags;
  RigidBody_SetFlag(self, Flags::CF_KINEMATIC_OBJECT, kinematic);
}

float RigidBody_GetMass (RigidBody* self) {
  return self->mass;
}

void RigidBody_SetMass (RigidBody* self, float mass) {
  self->mass = mass;
  RigidBody_RecalculateInertia(self);
}

void RigidBody_GetPos (RigidBody* self, Vec3f* pos) {
  btTransform transform = RigidBody_GetWorldTransform(self);
  *pos = Vec3f_FromBullet(transform.getOrigin());
}

void RigidBody_GetPosLocal (RigidBody* self, Vec3f* pos) {
  if (!RigidBody_IsChild(self))
    Fatal("RigidBody_GetPosLocal: Only allowed on children.");

  float            pScale   = RigidBody_GetScale(self->parent);
  CollisionShape*  cmpShape = CollisionShape_GetCached(self->iCompoundShape);
  btCompoundShape* compound = cmpShape->compound.handle;
  btTransform& transform = compound->getChildTransform(self->iCompound);
  *pos = Vec3f_FromBullet((1.0f / pScale) * transform.getOrigin());
}

void RigidBody_SetPos (RigidBody* self, Vec3f* pos) {
  if (RigidBody_IsChild(self))
    Fatal("RigidBody_SetPos: Not allowed on children.");

  btRigidBody* rigidBody = self->handle;
  btTransform  transform = rigidBody->getWorldTransform();
  transform.setOrigin(Vec3f_ToBullet(pos));
  rigidBody->setWorldTransform(transform);
}

void RigidBody_SetPosLocal (RigidBody* self, Vec3f* pos) {
  if (!RigidBody_IsChild(self))
    Fatal("RigidBody_SetPosLocal: Only allowed on children.");

  float            pScale   = RigidBody_GetScale(self->parent);
  CollisionShape*  cmpShape = CollisionShape_GetCached(self->iCompoundShape);
  btCompoundShape* compound = cmpShape->compound.handle;
  btTransform transform = compound->getChildTransform(self->iCompound);
  transform.setOrigin(pScale * Vec3f_ToBullet(pos));
  compound->updateChildTransform(self->iCompound, transform);
}

void RigidBody_GetBoundingBox (RigidBody* self, Box3f* box) {
  CollisionShape* shape     = CollisionShape_GetCached(self->iShape);
  btTransform     transform = RigidBody_GetWorldTransform(self);

  btVector3 min, max;
  shape->base.handle->getAabb(transform, min, max);
  box->lower = Vec3f_FromBullet(min);
  box->upper = Vec3f_FromBullet(max);
}

void RigidBody_GetBoundingBoxCompound (RigidBody* self, Box3f* box) {
  if (!RigidBody_IsParent(self))
    Fatal("RigidBody_GetBoundingBoxCompound: Only enabled for parents.");

  CollisionShape* cmpShape  = CollisionShape_GetCached(self->iCompoundShape);
  btTransform     transform = RigidBody_GetWorldTransform(self);

  btVector3 min, max;
  cmpShape->base.handle->getAabb(transform, min, max);
  box->lower = Vec3f_FromBullet(min);
  box->upper = Vec3f_FromBullet(max);
}

void RigidBody_GetBoundingBoxLocal (RigidBody* self, Box3f* box) {
  CollisionShape* shape     = CollisionShape_GetCached(self->iShape);
  btTransform     transform = btTransform::getIdentity();

  btVector3 min, max;
  shape->base.handle->getAabb(transform, min, max);
  box->lower = Vec3f_FromBullet(min);
  box->upper = Vec3f_FromBullet(max);
}

void RigidBody_GetBoundingBoxLocalCompound (RigidBody* self, Box3f* box) {
  if (!RigidBody_IsParent(self))
    Fatal("RigidBody_GetBoundingBoxLocalCompound: Only enabled for parents.");

  CollisionShape* cmpShape  = CollisionShape_GetCached(self->iCompoundShape);
  btTransform     transform = btTransform::getIdentity();

  btVector3 min, max;
  cmpShape->base.handle->getAabb(transform, min, max);
  box->lower = Vec3f_FromBullet(min);
  box->upper = Vec3f_FromBullet(max);
}

float RigidBody_GetBoundingRadius (RigidBody* self) {
  CollisionShape* shape = CollisionShape_GetCached(self->iShape);

  float radius = 0;
  switch (shape->type) {
    /* NOTE : btSphereShape doesn't override the virtual function
              btCollisionShape::getBoundingSphere so it falls back to
              calculating a sphere that encompases the local bounding box that
              encompases the original sphere. Lovely. */
    case CollisionShapeType_Sphere: {
      radius = shape->scale * shape->sphere.radius;
      break;
    }

    default: {
      btVector3 center;
      shape->base.handle->getBoundingSphere(center, radius);
      radius += center.length();
      break;
    }
  }
  return radius;
}

float RigidBody_GetBoundingRadiusCompound (RigidBody* self) {
  if (!RigidBody_IsParent(self))
    Fatal("RigidBody_GetBoundingBoxCompound: Only enabled for parents.");

  CollisionShape* cmpShape = CollisionShape_GetCached(self->iCompoundShape);
  float radius;
  btVector3 center;
  cmpShape->base.handle->getBoundingSphere(center, radius);
  radius += center.length();
  return radius;
}

RigidBody* RigidBody_GetParentBody (RigidBody* self) {
  return self->parent == self ? 0 : self->parent;
}

void RigidBody_SetRestitution (RigidBody* self, float restitution) {
  btRigidBody* rigidBody = self->handle;
  rigidBody->setRestitution(restitution);
}

void RigidBody_GetRot (RigidBody* self, Quat* rot) {
  btTransform transform = RigidBody_GetWorldTransform(self);
  *rot = Quat_FromBullet(transform.getRotation());
}

void RigidBody_GetRotLocal (RigidBody* self, Quat* rot) {
  if (!RigidBody_IsChild(self))
    Fatal("RigidBody_GetRotLocal: Only allowed on children.");

  CollisionShape*  cmpShape = CollisionShape_GetCached(self->iCompoundShape);
  btCompoundShape* compound = cmpShape->compound.handle;
  btTransform& transform = compound->getChildTransform(self->iCompound);
  *rot = Quat_FromBullet(transform.getRotation());
}

void RigidBody_SetRot (RigidBody* self, Quat* rot) {
  if (RigidBody_IsChild(self))
    Fatal("RigidBody_SetRot: Not allowed on children.");

  btRigidBody* rigidBody = self->handle;
  btTransform  transform = rigidBody->getWorldTransform();
  transform.setRotation(Quat_ToBullet(rot));
  rigidBody->setWorldTransform(transform);
}

void RigidBody_SetRotLocal (RigidBody* self, Quat* rot) {
  if (!RigidBody_IsChild(self))
    Fatal("RigidBody_SetRotLocal: Only allowed on children.");

  CollisionShape*  cmpShape = CollisionShape_GetCached(self->iCompoundShape);
  btCompoundShape* compound = cmpShape->compound.handle;
  btTransform transform = compound->getChildTransform(self->iCompound);
  transform.setRotation(Quat_ToBullet(rot));
  compound->updateChildTransform(self->iCompound, transform);
}

float RigidBody_GetScale (RigidBody* self) {
  /* NOTE : Only uniform scale is supported. */
  CollisionShape* shape = CollisionShape_GetCached(self->iShape);
  return shape->scale;
}

void RigidBody_SetScale (RigidBody* self, float scale) {
  /* NOTE : Only uniform scale is supported. */

  /* NOTE : Since scale is not inherited and the Bullet API for scaling a
            compound shape sucks, when a parent object is rescaled we only
            rescale its individual shape. */

  /* NOTE : We scale the positions of children to maintain their position
            relative to the parent. */

  btRigidBody* rigidBody = self->handle;

  CollisionShape shapeDef = *CollisionShape_GetCached(self->iShape);
  float scaleRatio = scale / shapeDef.scale;
  shapeDef.scale = scale;
  CollisionShape* shape = CollisionShape_Create(shapeDef);

  if (!RigidBody_IsCompound(self)) {
    rigidBody->setCollisionShape(shape->base.handle);
    self->iShape = shape->iShape;
    RigidBody_RecalculateInertia(self);

  } else {
    CollisionShape*  cmpShape = CollisionShape_GetCached(self->iCompoundShape);
    btCompoundShape* compound = cmpShape->compound.handle;

    //Children keep the same relative position
    if (RigidBody_IsParent(self)) {
      RigidBody* child = self->next;
      while (child) {
        btTransform& cTrans = compound->getChildTransform(child->iCompound);
        cTrans.getOrigin() *= scaleRatio;
        compound->updateChildTransform(child->iCompound, cTrans, false);
        child = child->next;
      }
    }

    btTransform transform = compound->getChildTransform(self->iCompound);
    compound->removeChildShapeByIndex(self->iCompound);
    compound->addChildShape(transform, shape->base.handle);
    int iLast = compound->getNumChildShapes() - 1;
    { /* HACK : btCompoundShape does a 'remove fast' internally. */
      RigidBody* last = self->parent;
      while (last->iCompound != iLast) last = last->next;
      last->iCompound = self->iCompound;
    }
    self->iCompound = iLast;
    self->iShape    = shape->iShape;

    /* NOTE: removeChildShape calls recalculateLocalAabb but
             removeChildShapeByIndex does not. addChildShape updates the AABB
             with the new shape only, it is not a full recalculation. So, if our
             new scalle is smaller than our old scale the AABB won't shrink as
             expected. Thus, we need to force a full recalculate. */
    compound->recalculateLocalAabb();
    if (RigidBody_IsParent(self))
      RigidBody_RecalculateInertia(self);
  }
}

void RigidBody_SetSleepThreshold (RigidBody* self, float linear, float angular) {
  btRigidBody* rigidBody = self->handle;
  rigidBody->setSleepingThresholds(linear, angular);
}

float RigidBody_GetSpeed (RigidBody* self) {
  if (RigidBody_IsChild(self))
    Fatal("RigidBody_GetSpeed: Not supported on children.");

  btRigidBody* rigidBody = self->handle;
  float speed = rigidBody->getLinearVelocity().length();
  return speed;
}

void RigidBody_GetVelocity (RigidBody* self, Vec3f* velocity) {
  if (RigidBody_IsChild(self))
    Fatal("RigidBody_GetVelocity: Not supported on children.");

  btRigidBody* rigidBody = self->handle;
  *velocity = Vec3f_FromBullet(rigidBody->getLinearVelocity());
}

void RigidBody_GetVelocityA (RigidBody* self, Vec3f* velocityA) {
  if (RigidBody_IsChild(self))
    Fatal("RigidBody_GetVelocityA: Not supported on children.");

  btRigidBody* rigidBody = self->handle;
  *velocityA = Vec3f_FromBullet(rigidBody->getAngularVelocity());
}

/* NOTE : We assume the parent has identity position and rotation within the
          compound. */

/* NOTE : Detach could be made O(1) if a doubly linked list were used in place
          of the singly linked list. (Attach is O(1) in both cases.) */

/* NOTE : Free strategy: Freed parents will free all children. Children cannot
          be freed directly. Rigidbodies will not be removed from physics when
          freed, this must be done manually. */

/* NOTE : Coumans says btConvexHullShape is more efficient than
          btConvexPointCloudShape.
          https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=3102 */

/* NOTE : btConvexHullShape duplicates the vertex positions.
          btConvexTriangleMeshShape does not, but the class comment says
          it's less efficient. */

/* NOTE : btShapeHull can be used to reduce the number of hull vertices.
          http://www.bulletphysics.org/mediawiki-1.5.8/index.php?title=BtShapeHull_vertex_reduction_utility */

/* NOTE : btConvexHull, btCompoundShape, and btUniformScaling all have shit
          implementations and APIs. I'm quite sure they are hurting performance
          and generally being obtuse pains in the dick. Josh, when you get
          annoyed with Bullet and start trying to optimize it, I'd strongly
          consider implementing these from scratch. */

/* TODO : Pool allocate Bullet objects if possible. Does Bullet have a pooling
          mechanism? Use our own arrays? */

/* TODO : btUniformScalingShape::getAabb looks like it might be quite slow. It's
          not clear why it doesn't simply forward the call to the wrapped shape
          and scale the result. */

#include "CollisionShape.h"
#include "PhysicsDefs.h"
#include "RigidBody.h"
#include "Trigger.h"

inline static bool Trigger_IsAttached (Trigger* self) {
  return self->parent != 0;
}

inline static void Trigger_SetFlag (Trigger* self, int flag, bool enable) {
  int flags = self->handle->getCollisionFlags();
  flags = enable ? flags | flag : flags & ~flag;
  self->handle->setCollisionFlags(flags);
}

inline static Trigger* Trigger_Create (CollisionShape* shape) {
  Trigger* self = MemNewZero(Trigger);
  self->type           = PhysicsType_Trigger;
  self->handle         = new GhostObject();
  self->iShape         = shape->iShape;
  self->collisionGroup = CollisionGroup_Trigger;
  self->collisionMask  = CollisionMask_NoTriggers;

  self->handle->setCollisionShape(shape->base.handle);
  self->handle->setUserPointer(self);

  typedef btCollisionObject::CollisionFlags Flags;
  Trigger_SetFlag(self, Flags::CF_NO_CONTACT_RESPONSE, true);

  return self;
}

Trigger* Trigger_CreateBox (Vec3f* halfExtents) {
  CollisionShape* shape = CollisionShape_CreateBox(halfExtents);
  return Trigger_Create(shape);
}

void Trigger_Free (Trigger* self) {
  if (self->physics)
    Fatal("Trigger_Free: Object is still present in Physics.");

  CollisionShape* shape = CollisionShape_GetCached(self->iShape);
  CollisionShape_Free(shape);
  MemFree(self);
}

void Trigger_GetBoundingBox (Trigger* self, Box3f* box) {
  CollisionShape* shape     = CollisionShape_GetCached(self->iShape);
  btTransform     transform = self->handle->getWorldTransform();

  btVector3 min, max;
  shape->base.handle->getAabb(transform, min, max);
  box->lower = Vec3f_FromBullet(min);
  box->upper = Vec3f_FromBullet(max);
}

int Trigger_GetContentsCount (Trigger* self) {
  return self->handle->getNumOverlappingObjects();
}

RigidBody* Trigger_GetContents (Trigger* self, int index) {
  btCollisionObject* collisionObject = self->handle->getOverlappingObject(index);
  RigidBody*         rigidBody       = (RigidBody*) collisionObject->getUserPointer();
  return rigidBody;
}

void Trigger_Attach (Trigger* self, RigidBody* parent, Vec3f* pos) {
  if (self->parent)
    Fatal("Trigger_Attach: Trigger is already attached to an object.");

  self->handle->setIgnoreCollisionCheck(parent->handle, true);
  self->parent         = parent;
  self->transformLocal = btTransform(btQuaternion::getIdentity(), Vec3f_ToBullet(pos));

  self->next = parent->triggers;
  parent->triggers = self;
}

void Trigger_Detach (Trigger* self, RigidBody* parent) {
  if (!self->parent)
    Fatal("Trigger_Detach: Trigger is not attached to an object.");
  if (self->parent != parent)
    Fatal("Trigger_Detach: Trigger is attached to a different object.");

  parent->triggers = self->next;
  self->next = 0;

  self->handle->setIgnoreCollisionCheck(parent->handle, false);
  self->parent         = 0;
  self->transformLocal = btTransform::getIdentity();
}

void Trigger_SetCollisionMask (Trigger* self, int mask) {
  if (HAS_FLAG(mask, CollisionGroup_Trigger))
    Fatal("Trigger_SetCollisionMask: Triggers may not collide with other CollisionGroup_Triggers");

  self->collisionMask = mask;

  if (self->physics) {
    btBroadphaseProxy* proxy = self->handle->getBroadphaseHandle();
    proxy->m_collisionFilterMask = mask;
  }
}

void Trigger_SetPos (Trigger* self, Vec3f* pos) {
  if (Trigger_IsAttached(self))
    Fatal("Trigger_SetPos: Not allowed when attached to a RigidBody.");

  btTransform transform = self->handle->getWorldTransform();
  transform.setOrigin(Vec3f_ToBullet(pos));
  self->handle->setWorldTransform(transform);
}

void Trigger_SetPosLocal (Trigger* self, Vec3f* pos) {
  if (!Trigger_IsAttached(self))
    Fatal("Trigger_SetPosLocal: Only allowed when attached to a RigidBody.");

  self->transformLocal.setOrigin(Vec3f_ToBullet(pos));
}

RigidBody* Trigger_GetParent (Trigger* self) {
  return self->parent;
}

void Trigger_Update (Trigger* self) {
  if (Trigger_IsAttached(self)) {
    Assert(self->parent->physics);
    /* TODO: Should only be setting position! Whoops! */
    btTransform transform = self->parent->handle->getWorldTransform();
    self->handle->setWorldTransform(transform * self->transformLocal);
  }
}

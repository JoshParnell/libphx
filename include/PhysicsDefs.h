#ifndef PHX_PhysicsDefs
#define PHX_PhysicsDefs

#include "Bullet.h"
#include "Common.h"

typedef uint8 PhysicsType;
const PhysicsType PhysicsType_Null      = 0;
const PhysicsType PhysicsType_RigidBody = 1;
const PhysicsType PhysicsType_Trigger   = 2;

struct RigidBody {
  PhysicsType  type;
  btRigidBody* handle;         // Always references *this* object, even when part of a compound
  int          iShape;         // Always references *this* object, even when part of a compound
  int          collisionGroup; // Which group this object is part of
  int          collisionMask;  // Which other groups this object collides with
  float        mass;           // For GetMass and calculating inertia (mass is not stored in btRigidBody)

  int          iCompound;      // The index within the compound (-1 when not a compound)
  int          iCompoundShape; // The compound shape (-1 when not a compound)
  RigidBody*   parent;         // The parent object in the compound (null when not a compound)
  RigidBody*   next;           // The next object in the compound (null when this is the last child or not a compound)

  Physics*     physics;
  Trigger*     triggers;
  Matrix       mat;
};

struct Trigger {
  PhysicsType  type;
  GhostObject* handle;
  int          iShape;
  int          collisionGroup;
  int          collisionMask;

  RigidBody*   parent;
  Trigger*     next;
  btTransform  transformLocal;

  Physics*     physics;
};

#endif

/* NOTE : We have to store mass for GetMass and recalculating inertia. */
/* NOTE : We need access to all children to free when freeing a parent. */
/* NOTE : We need access to the parent to detch when freeing a child. */

/* NOTE : For 2 objects to collide the layer of each object must be present in
          the collidesWithLayers of the other object (i.e. it's symmetric). */

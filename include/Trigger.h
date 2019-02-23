#ifndef PHX_Trigger
#define PHX_Trigger

#include "Common.h"

/* --- Trigger -----------------------------------------------------------------
 *
 *   Trigger_Attach      : When attached to a RigidBody Triggers will have 1
 *                         frame of latency in their position. The transform of
 *                         the RigidBody is copied to the Trigger at the
 *                         beginning of each Physics_Update. This will include
 *                         manual RigidBody_SetPos, but will not not include the
 *                         pending kinematics update.
 *
 *   Trigger_GetContents : Will only include the parent object when a compound
 *                         is within the trigger.
 *
 * -------------------------------------------------------------------------- */

PHX_API Trigger*    Trigger_CreateBox         (Vec3f* halfExtents);
PHX_API void        Trigger_Free              (Trigger*);

PHX_API void        Trigger_Attach            (Trigger*, RigidBody*, Vec3f*);
PHX_API void        Trigger_Detach            (Trigger*, RigidBody*);

PHX_API void        Trigger_GetBoundingBox    (Trigger*, Box3f*);
PHX_API int         Trigger_GetContentsCount  (Trigger*);
PHX_API RigidBody*  Trigger_GetContents       (Trigger*, int);
PHX_API void        Trigger_SetCollisionMask  (Trigger*, int);

PHX_API void        Trigger_SetPos            (Trigger*, Vec3f*);
PHX_API void        Trigger_SetPosLocal       (Trigger*, Vec3f*);

PRIVATE RigidBody*  Trigger_GetParent  (Trigger*);
PRIVATE void        Trigger_Update     (Trigger*);

#endif

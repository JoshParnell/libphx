#ifndef PHX_RigidBody
#define PHX_RigidBody

#include "Common.h"

/* --- RigidBody ---------------------------------------------------------------
 *
 *   The following API functions are disabled for parent objects:
 *   RigidBody_GetPosLocal, RigidBody_SetPosLocal, RigidBody_GetRotLocal, and
 *   RigidBody_SetRotLocal.
 *
 *   The following API functions are disabled for child objects:
 *   RigidBody_Free, RigidBody_ApplyForce, RigidBody_ApplyTorque,
 *   RigidBody_GetSpeed, RigidBody_GetVelocity, RigidBody_GetVelocityA,
 *   RigidBody_SetPos, and RigidBody_SetRot.
 *
 *   The following API functions only have an effect once the child is removed
 *   from its parent: RigidBody_SetCollidable, RigidBody_SetCollisionGroup,
 *   RigidBody_SetCollisionMask, RigidBody_SetDrag, RigidBody_SetFriction,
 *   RigidBody_SetKinematic, RigidBody_SetRestitution, and
 *   RigidBody_SetSleepThreshold.
 *
 *   The following API functions return information only about the current part
 *   when the object is part of a compound: RigidBody_GetBoundingBoxLocal,
 *   RigidBody_GetBoundingBox, and RigidBody_GetBoundingRadius.
 *
 *   The following API functions are only enabled for compound objects:
 *   RigidBody_GetBoundingBoxCompound, RigidBody_GetBoundingBoxLocalCompound,
 *   and RigidBody_GetBoundingRadiusCompound.
 *
 *   The local coordinate space of a child object is not scaled by the parent.
 *   However, the position of the child will be multiplied by the parents scale.
 *   Thus, the scale of the parent does not affect the size of the child and
 *   local position is always 'relative to the parent'. A position of (1, 1, -1)
 *   will always correspond to a point that will roughly coincide with the
 *   right-top-front corner of the parents bounding box (assuming the vertices
 *   of the mesh are contained in a cube that goes from o(-1, -1, -1) to
 *   (1, 1, 1)). When a parent is scaled the positions of children will be
 *   multiplied in order to maintain the same relative position.
 *
 *     RigidBody_Free             : Automatically frees all attached Triggers
 *                                  when called on a parent. Automatically frees
 *                                  all attached children and their Triggers
 *                                  when called on a parent. This function is
 *                                  O(M*N) for parents.
 *     RigidBody_Attach           : Only a single level of attachment is
 *                                  supported. Child objects do not affect the
 *                                  mass or inertia of the parent. Position is
 *                                  relative to the unscaled parent. i.e. it
 *                                  will be multiplied by the current scale.
 *                                  This function is O(1). Warning: if one
 *                                  object is attached to another and a third
 *                                  object happens to be between them this may
 *                                  trap the third object. The same issue may
 *                                  occur when spawning one compound inside
 *                                  another.
 *     RigidBody_Detach           : This function is O(2N).
 *     RigidBody_GetToLocalMatrix : Requires a 3x3 transpose, 3x3 vs 3x1
 *                                  multiplication, 4x4 copy, 3x3 vs 1x1
 *                                  multiplication, and a 4x4 transpose.
 *                                  Children require an additional 3x3 vs 3x3
 *                                  multiplication and 3v3 vs 3x1 multiplcation.
 *     RigidBody_GetToWorldMatrix : Requires a 4x4 copy, 3x3 vs 1x1
 *                                  multiplication, and a 4x4 transpose.
 *                                  Children require an additional 3x3 vs 3x3
 *                                  multiplication and 3v3 vs 3x1 multiplcation.
 *     RigidBody_SetCollidable    : When disabled, the object will pass through
 *                                  others without colliding and will not be
 *                                  returned from ray or shape casts.
 *     RigidBody_SetMass          : The mass of child objects does not affect
 *                                  the mass or inertia of the parent.
 *     RigidBody_GetPos           : Children return the parent position.
 *     RigidBody_GetPosLocal      : Local coordinates are relative to the parent
 *                                  *before* scaling. Requires an additional 3x3
 *                                  vs 3x3 multiplication and 3v3 vs 3x1
 *                                  multiplcation.
 *     RigidBody_SetPosLocal      : Local coordinates are relative to the parent
 *                                  *before* scaling. The given position will be
 *                                  multiplied by the parent's scale.
 *     RigidBody_GetRot           : Children require an additional 3x3 vs 3x3
 *                                  multiplication and 3v3 vs 3x1 multiplcation.
 *     RigidBody_SetScale         : When called on a parent object the positions
 *                                  of all children will be multiplied such that
 *                                  they retain the same relative position.
 *                                  Child scale is not affected by parent scale
 *                                  (i.e. it is not inherited). This function is
 *                                  O(3N).
 *
 * -------------------------------------------------------------------------- */

PHX_API RigidBody*  RigidBody_CreateBox                    ();
PHX_API RigidBody*  RigidBody_CreateBoxFromMesh            (Mesh*);
PHX_API RigidBody*  RigidBody_CreateSphere                 ();
PHX_API RigidBody*  RigidBody_CreateSphereFromMesh         (Mesh*);
PHX_API RigidBody*  RigidBody_CreateHullFromMesh           (Mesh*);
PHX_API void        RigidBody_Free                         (RigidBody*);

PHX_API void        RigidBody_ApplyForce                   (RigidBody*, Vec3f*);
PHX_API void        RigidBody_ApplyTorque                  (RigidBody*, Vec3f*);

PHX_API void        RigidBody_Attach                       (RigidBody*, RigidBody* other, Vec3f*, Quat*);
PHX_API void        RigidBody_Detach                       (RigidBody*, RigidBody* other);

PHX_API void        RigidBody_GetBoundingBox               (RigidBody*, Box3f*);
PHX_API void        RigidBody_GetBoundingBoxCompound       (RigidBody*, Box3f*);
PHX_API void        RigidBody_GetBoundingBoxLocal          (RigidBody*, Box3f*);
PHX_API void        RigidBody_GetBoundingBoxLocalCompound  (RigidBody*, Box3f*);
PHX_API float       RigidBody_GetBoundingRadius            (RigidBody*);
PHX_API float       RigidBody_GetBoundingRadiusCompound    (RigidBody*);

PHX_API RigidBody*  RigidBody_GetParentBody                (RigidBody*);
PHX_API float       RigidBody_GetSpeed                     (RigidBody*);
PHX_API Matrix*     RigidBody_GetToLocalMatrix             (RigidBody*);
PHX_API Matrix*     RigidBody_GetToWorldMatrix             (RigidBody*);
PHX_API void        RigidBody_GetVelocity                  (RigidBody*, Vec3f*);
PHX_API void        RigidBody_GetVelocityA                 (RigidBody*, Vec3f*);

PHX_API void        RigidBody_SetCollidable                (RigidBody*, bool);
PHX_API void        RigidBody_SetCollisionGroup            (RigidBody*, int);
PHX_API void        RigidBody_SetCollisionMask             (RigidBody*, int);
PHX_API void        RigidBody_SetDrag                      (RigidBody*, float linear, float angular);
PHX_API void        RigidBody_SetFriction                  (RigidBody*, float);
PHX_API void        RigidBody_SetKinematic                 (RigidBody*, bool);
PHX_API void        RigidBody_SetRestitution               (RigidBody*, float);
PHX_API void        RigidBody_SetSleepThreshold            (RigidBody*, float linear, float angular);

PHX_API float       RigidBody_GetMass                      (RigidBody*);
PHX_API void        RigidBody_SetMass                      (RigidBody*, float);
PHX_API void        RigidBody_GetPos                       (RigidBody*, Vec3f*);
PHX_API void        RigidBody_GetPosLocal                  (RigidBody*, Vec3f*);
PHX_API void        RigidBody_SetPos                       (RigidBody*, Vec3f*);
PHX_API void        RigidBody_SetPosLocal                  (RigidBody*, Vec3f*);
PHX_API void        RigidBody_GetRot                       (RigidBody*, Quat*);
PHX_API void        RigidBody_GetRotLocal                  (RigidBody*, Quat*);
PHX_API void        RigidBody_SetRot                       (RigidBody*, Quat*);
PHX_API void        RigidBody_SetRotLocal                  (RigidBody*, Quat*);
PHX_API float       RigidBody_GetScale                     (RigidBody*);
PHX_API void        RigidBody_SetScale                     (RigidBody*, float);

PRIVATE RigidBody*  RigidBody_GetPart     (RigidBody*, int);
PRIVATE bool        RigidBody_IsChild     (RigidBody*);
PRIVATE bool        RigidBody_IsCompound  (RigidBody*);
PRIVATE bool        RigidBody_IsParent    (RigidBody*);

#endif

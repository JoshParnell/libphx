#include "Basis3.h"
#include "PhxMath.h"

static void RotateTangentSpace (Vec3f* NO_ALIAS e1, Vec3f* NO_ALIAS e2, float angle) {
  float c = Cos(angle);
  float s = Sin(angle);
  Vec3f n1 = Vec3f_Add(Vec3f_Muls(*e1, c), Vec3f_Muls(*e2, -s));
  Vec3f n2 = Vec3f_Add(Vec3f_Muls(*e1, s), Vec3f_Muls(*e2,  c));
  *e1 = n1;
  *e2 = n2;
}

void Basis3_Rotate (Basis3* self, Vec3f angle) {
  if (NonZero(angle.x)) RotateTangentSpace(&self->z, &self->y, angle.x);
  if (NonZero(angle.y)) RotateTangentSpace(&self->x, &self->z, angle.y);
  if (NonZero(angle.z)) RotateTangentSpace(&self->y, &self->x, angle.z);
}

void Basis3_Orthogonalize (Basis3* self) {
  self->z = Vec3f_Normalize(self->z);
  self->y = Vec3f_Normalize(Vec3f_Reject(self->y, self->z));
  self->x = Vec3f_Cross(self->z, self->y);
}

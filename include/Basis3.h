#ifndef PHX_Basis3
#define PHX_Basis3

#include "Common.h"
#include "Vec3.h"

struct Basis3 {
  Vec3f x;
  Vec3f y;
  Vec3f z;
};

inline Basis3 Basis3_Create() {
  Basis3 self = { { 1, 0, 0 }, { 0, 1, 0 },  { 0, 0, 1 } };
  return self;
}

void  Basis3_Orthogonalize  (Basis3*);
void  Basis3_Rotate         (Basis3*, Vec3f angle);

#endif

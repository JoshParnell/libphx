#include "GameObjectType.h"

struct GameObjectField {
  GameObjectField* prev;
  cstr name;
  uint32 type;
};

struct GameObjectType {
  uint32 id;
  cstr name;
  GameObjectField* fields;
};

GameObjectType* GameObjectType_Create () {
  NYI;
  return 0;
};

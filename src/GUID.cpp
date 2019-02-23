#include "GUID.h"

uint64 nextID = 1;

uint64 GUID_Create () {
  return nextID++;
}

bool GUID_Exists (uint64 id) {
  return id < nextID && id != 0;
}

void GUID_Reset () {
  nextID = 1;
}

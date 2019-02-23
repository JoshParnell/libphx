#include "ArrayList.h"
#include "Box3.h"
#include "Hash.h"
#include "HashGrid.h"
#include "MemPool.h"
#include "PhxMemory.h"
#include "PhxMath.h"
#include "Profiler.h"

#define OPT_SPARSEUPDATE 1

struct HashGridElem {
  uint64 version;
  void* object;
  int32 lower[3];
  int32 upper[3];
};

struct HashGridCell {
  uint64 version;
  ArrayList(HashGridElem*, elems);
};

struct HashGrid {
  uint64 version;
  HashGridCell* cells;
  MemPool* elemPool;
  uint32 cellCount;
  float cellSize;
  uint32 mask;
  ArrayList(void*, results);
};

HashGrid* HashGrid_Create (float cellSize, uint32 cellCount) {
  uint32 logCount = 0;
  while (cellCount > 1) {
    cellCount /= 2;
    logCount++;
  }
  cellCount = 1 << logCount;

  HashGrid* self = MemNew(HashGrid);
  self->version = 0;
  self->cells = MemNewArrayZero(HashGridCell, cellCount);
  self->elemPool = MemPool_Create(sizeof(HashGridElem), 0x1000U / sizeof(HashGridElem));
  self->cellCount = cellCount;
  self->cellSize = cellSize;
  self->mask = (1 << logCount) - 1;
  ArrayList_Init(self->results);
  for (uint32 i = 0; i < cellCount; ++i)
    ArrayList_Init(self->cells[i].elems);
  return self;
}

void HashGrid_Free (HashGrid* self) {
  ArrayList_Free(self->results);
  for (uint32 i = 0; i < self->cellCount; ++i)
    ArrayList_Free(self->cells[i].elems);
  MemPool_Free(self->elemPool);
  MemFree(self->cells);
  MemFree(self);
}

inline static HashGridCell* HashGrid_GetCell (HashGrid* self, int32 x, int32 y, int32 z) {
  int32 p[3] = { x, y, z };
  uint64 hash = Hash_XX64(p, sizeof(p), 0x0ULL);
  return self->cells + (hash & self->mask);
}

static void HashGrid_AddElem (HashGrid* self, HashGridElem* elem) {
  self->version++;
  for (int32 x = elem->lower[0]; x <= elem->upper[0]; ++x)
  for (int32 y = elem->lower[1]; y <= elem->upper[1]; ++y)
  for (int32 z = elem->lower[2]; z <= elem->upper[2]; ++z) {
    HashGridCell* cell = HashGrid_GetCell(self, x, y, z);

    /* If cell version is out-of-date, we need to insert. Otherwise, we have
     * already inserted into this cell (e.g., we have encountered a modulus
     * hash collision) and should not insert a duplicate. */
    if (cell->version != self->version) {
      cell->version = self->version;
      ArrayList_Append(cell->elems, elem);
    }
  }
}

static void HashGrid_RemoveElem (HashGrid* self, HashGridElem* elem) {
  self->version++;
  for (int32 x = elem->lower[0]; x <= elem->upper[0]; ++x)
  for (int32 y = elem->lower[1]; y <= elem->upper[1]; ++y)
  for (int32 z = elem->lower[2]; z <= elem->upper[2]; ++z) {
    HashGridCell* cell = HashGrid_GetCell(self, x, y, z);
    if (cell->version != self->version) {
      cell->version = self->version;
      ArrayList_RemoveFast(cell->elems, elem);
    }
  }
}

inline static int32 HashGrid_ToLocal (HashGrid* self, float x) {
  return (int32)Floor(x / self->cellSize);
}

HashGridElem* HashGrid_Add (HashGrid* self, void* object, Box3f const* box) {
  HashGridElem* elem = (HashGridElem*)MemPool_Alloc(self->elemPool);
  elem->object = object;
  elem->lower[0] = HashGrid_ToLocal(self, box->lower.x);
  elem->lower[1] = HashGrid_ToLocal(self, box->lower.y);
  elem->lower[2] = HashGrid_ToLocal(self, box->lower.z);
  elem->upper[0] = HashGrid_ToLocal(self, box->upper.x);
  elem->upper[1] = HashGrid_ToLocal(self, box->upper.y);
  elem->upper[2] = HashGrid_ToLocal(self, box->upper.z);
  HashGrid_AddElem(self, elem);
  return elem;
}

void HashGrid_Clear (HashGrid* self) {
  self->version = 0;
  for (uint32 i = 0; i < self->cellCount; ++i) {
    ArrayList_Clear(self->cells[i].elems);
    self->cells[i].version = 0;
  }
  MemPool_Clear(self->elemPool);
  ArrayList_Clear(self->results);
}

void HashGrid_Remove (HashGrid* self, HashGridElem* elem) {
  HashGrid_RemoveElem(self, elem);
  MemPool_Dealloc(self->elemPool, elem);
}

void HashGrid_Update (HashGrid* self, HashGridElem* elem, Box3f const* box) {
  FRAME_BEGIN;
  int32 lower[3] = {
    HashGrid_ToLocal(self, box->lower.x),
    HashGrid_ToLocal(self, box->lower.y),
    HashGrid_ToLocal(self, box->lower.z),
  };

  int32 upper[3] = {
    HashGrid_ToLocal(self, box->upper.x),
    HashGrid_ToLocal(self, box->upper.y),
    HashGrid_ToLocal(self, box->upper.z),
  };

  if (lower[0] == elem->lower[0] && upper[0] == elem->upper[0] &&
      lower[1] == elem->lower[1] && upper[1] == elem->upper[1] &&
      lower[2] == elem->lower[2] && upper[2] == elem->upper[2]) {
    FRAME_END;
    return;
  }

#if OPT_SPARSEUPDATE
  int32 lowerUnion[3] = {
    Min(lower[0], elem->lower[0]),
    Min(lower[1], elem->lower[1]),
    Min(lower[2], elem->lower[2]),
  };

  int32 upperUnion[3] = {
    Max(upper[0], elem->upper[0]),
    Max(upper[1], elem->upper[1]),
    Max(upper[2], elem->upper[2]),
  };

  uint64 vRemove = ++self->version;
  uint64 vAdd = ++self->version;

  /* TODO OPT : Can skip entire ranges rather than by splitting the inPrev and
   *            inCurr computation along each axis. */
  for (int32 x = lowerUnion[0]; x <= upperUnion[0]; ++x)
  for (int32 y = lowerUnion[1]; y <= upperUnion[1]; ++y)
  for (int32 z = lowerUnion[2]; z <= upperUnion[2]; ++z) {
    bool inPrev =
      elem->lower[0] <= x &&
      elem->lower[1] <= y &&
      elem->lower[2] <= z &&
      x <= elem->upper[0] &&
      y <= elem->upper[1] &&
      z <= elem->upper[2];

    bool inCurr =
      lower[0] <= x &&
      lower[1] <= y &&
      lower[2] <= z &&
      x <= upper[0] &&
      y <= upper[1] &&
      z <= upper[2];

    /* Early out: cell is part of both previous and new bounding box, no change
     * required. */
    if (inPrev && inCurr)
      continue;

    HashGridCell* cell = HashGrid_GetCell(self, x, y, z);

    /* Early out: cell has already had elem inserted, no update required. */
    if (cell->version == vAdd)
      continue;
    
    /* Early out: cell has already had elem removed and this is not part of the
     * insertion frontier. */
    if (cell->version == vRemove && inPrev)
      continue;

    /* inPrev but not inCurr -> remove elem and mark cell as having removed. */
    if (inPrev) {
      ArrayList_RemoveFast(cell->elems, elem);
      cell->version = vRemove;
      continue;
    }

    /* Final case: insertion frontier. We must add the elem to this cell. */
    if (cell->version != vRemove)
      ArrayList_RemoveFast(cell->elems, elem);
    ArrayList_Append(cell->elems, elem);
    cell->version = vAdd;
  }
  elem->lower[0] = lower[0];
  elem->lower[1] = lower[1];
  elem->lower[2] = lower[2];
  elem->upper[0] = upper[0];
  elem->upper[1] = upper[1];
  elem->upper[2] = upper[2];
#else
  HashGrid_RemoveElem(self, elem);
  elem->lower[0] = lower[0];
  elem->lower[1] = lower[1];
  elem->lower[2] = lower[2];
  elem->upper[0] = upper[0];
  elem->upper[1] = upper[1];
  elem->upper[2] = upper[2];
  HashGrid_AddElem(self, elem);
#endif

  FRAME_END;
}

void** HashGrid_GetResults (HashGrid* self) {
  return ArrayList_GetData(self->results);
}

int HashGrid_QueryBox (HashGrid* self, Box3f const* box) {
  ArrayList_Clear(self->results);
  self->version++;

  int32 lower[3] = {
    HashGrid_ToLocal(self, box->lower.x),
    HashGrid_ToLocal(self, box->lower.y),
    HashGrid_ToLocal(self, box->lower.z),
  };

  int32 upper[3] = {
    HashGrid_ToLocal(self, box->upper.x),
    HashGrid_ToLocal(self, box->upper.y),
    HashGrid_ToLocal(self, box->upper.z),
  };

  for (int32 x = lower[0]; x <= upper[0]; ++x)
  for (int32 y = lower[1]; y <= upper[1]; ++y)
  for (int32 z = lower[2]; z <= upper[2]; ++z) {
    HashGridCell* cell = HashGrid_GetCell(self, x, y, z);
    if (cell->version != self->version) {
      cell->version = self->version;
      for (int32 i = 0; i < ArrayList_GetSize(cell->elems); ++i) {
        HashGridElem* elem = ArrayList_Get(cell->elems, i);

        /* We must use per-element versioning here due to the fact that the same
         * element may exist in many distinct cells, hence we might encounter
         * it many times. We should only report it once regardless. */
        if (elem->version != self->version) {
          elem->version = self->version;
          ArrayList_Append(self->results, elem->object);
        }
      }
    }
  }

  return ArrayList_GetSize(self->results);
}

int HashGrid_QueryPoint (HashGrid* self, Vec3f const* p) {
  /* Since a point query is restricted to a single cell, we don't need to use
   * versioning here. */
  ArrayList_Clear(self->results);

  HashGridCell* cell = HashGrid_GetCell(self,
    HashGrid_ToLocal(self, p->x),
    HashGrid_ToLocal(self, p->y),
    HashGrid_ToLocal(self, p->z));

  for (int32 i = 0; i < ArrayList_GetSize(cell->elems); ++i) {
    HashGridElem* elem = ArrayList_Get(cell->elems, i);
    ArrayList_Append(self->results, elem->object);
  }

  return ArrayList_GetSize(self->results);
}

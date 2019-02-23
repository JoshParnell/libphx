#ifndef PHX_ArrayList
#define PHX_ArrayList
#define __FFI_IGNORE__

#include "PhxMemory.h"

/* --- ArrayList ---------------------------------------------------------------
 *
 *   A dynamic linear array with geometric allocation, yielding O(1) amortized
 *   append and O(1) random access (like std::vector).
 *
 *   Example idiomatic usage:
 *
 *     struct MyStruct {
 *       ArrayList(int, nums);
 *       ArrayList(double, roots);
 *       ...
 *     };
 *
 *     MyStruct* MyStruct_Create(...) {
 *       MyStruct* self = MemNew(MyStruct);
 *       ArrayList_Init(self->nums);
 *       ArrayList_Init(self->roots);
 *
 *       for (int i = 0; i < 100; ++i)
 *         ArrayList_Append(self->nums, i);
 *
 *       for (int i = 0; i < ArrayList_GetSize(self->nums); ++i) {
 *         int elem = ArrayList_Get(self->nums, i);
 *         ArrayList_Append(self->roots, Sqrt((double)elem));
 *       }
 *       ...
 *       return self;
 *    }
 *
 *    void MyStruct_Free(MyStruct* self) {
 *      ArrayList_Free(self->nums);
 *      ArrayList_Free(self->roots);
 *    }
 *
 * -------------------------------------------------------------------------- */

#define ArrayList(T, name)                                                     \
  int32 name##_size;                                                           \
  int32 name##_capacity;                                                       \
  T* name##_data

#define ArrayList_Init(name)                                                   \
  { name##_capacity = 0;                                                       \
    name##_size = 0;                                                           \
    name##_data = 0; }

#define ArrayList_Free(name)                                                   \
  MemFree(name##_data);

#define ArrayList_FreeEx(name, freeFn)                                         \
  { for (int i = 0; i < name##_size; ++i)                                      \
      freeFn(name##_data[i]);                                                  \
    MemFree(name##_data); }

#define ArrayList_FreeExPtr(name, freeFn)                                      \
  { for (int i = 0; i < name##_size; ++i)                                      \
      freeFn(name##_data + i);                                                 \
    MemFree(name##_data); }

#define ArrayList_Append(name, value)                                          \
  { ArrayList_Grow(name);                                                      \
    name##_data[name##_size++] = (value); }

#define ArrayList_ForEach(name, T, iter)                                       \
  for (T *iter = name##_data, *__iterend = name##_data + name##_size;          \
       iter < __iterend; ++iter)

#define ArrayList_ForEachI(name, index)                                        \
  for (int index = 0; index < name##_size; ++index)

#define ArrayList_ForEachReverse(name, T, iter)                                \
  for (T *iter = name##_data + name##_size - 1, *__iterbegin = name##_data;    \
       iter >= __iterbegin; --iter)

#define ArrayList_ForEachIReverse(name, index)                                 \
  for (int index = name##_size - 1; index >= 0; --index)

#define ArrayList_Grow(name)                                                   \
  IF_UNLIKELY (name##_capacity == name##_size) {                               \
    name##_capacity = name##_capacity ? name##_capacity * 2 : 1;               \
    size_t elemSize = sizeof(name##_data[0]);                                  \
    void** pData    = ((void**) &name##_data);                                 \
    *pData = MemRealloc(name##_data, name##_capacity * elemSize);              \
  }

#define ArrayList_Insert(name, value, index)                                   \
  { ArrayList_Grow(name);                                                      \
    if (i != name##_size - 1) {                                                \
      void*  curr     = name##_data + i + 0;                                   \
      void*  next     = name##_data + i + 1;                                   \
      size_t elemSize = sizeof(name##_data[0]);                                \
      MemMove(next, curr, (name##_size - 1 - i) * elemSize);                   \
    }                                                                          \
    name##_data[index] = (value);                                              \
    name##_size++; }

#define ArrayList_Pop(name)                                                    \
  --name##_size

#define ArrayList_PopRet(name)                                                 \
  name##_data[--name##_size]

#define ArrayList_RemoveAt(name, i)                                            \
  { if (i != name##_size - 1) {                                                \
      void*  curr     = name##_data + i + 0;                                   \
      void*  next     = name##_data + i + 1;                                   \
      size_t elemSize = sizeof(name##_data[0]);                                \
      MemMove(curr, next, (name##_size - 1 - i) * elemSize);                   \
    }                                                                          \
    name##_size--; }

#define ArrayList_RemoveAtFast(name, i)                                        \
  { name##_data[i] = name##_data[--name##_size]; }

#define ArrayList_Remove(name, elem)                                           \
  { for (int32 _i = 0; _i < name##_size; ++_i) {                               \
      IF_UNLIKELY (name##_data[_i] == elem) {                                  \
        if (_i != name##_size - 1) {                                           \
          void*  curr     = name##_data + _i + 0;                              \
          void*  next     = name##_data + _i + 1;                              \
          size_t elemSize = sizeof(name##_data[0]);                            \
          MemMove(curr, next, (name##_size - 1 - _i) * elemSize);              \
        }                                                                      \
        name##_size--;                                                         \
        break;                                                                 \
      }                                                                        \
  }}

#define ArrayList_RemoveWhere(name, T, predicate)                              \
  { for (int32 _i = 0; _i < name##_size; ++_i) {                               \
      T* x = &name##_data[_i];                                                 \
      IF_UNLIKELY (predicate) {                                                \
        for (int32 _j = _i; _j < name##_size - 1; ++_j)                        \
          name##_data[_j] = name##_data[_j + 1];                               \
        --name##_size;                                                         \
        break;                                                                 \
      }                                                                        \
  }}

#define ArrayList_RemoveFast(name, elem)                                       \
  { for (int32 _i = 0; _i < name##_size; ++_i) {                               \
      IF_UNLIKELY (name##_data[_i] == elem) {                                  \
        name##_data[_i] = name##_data[--name##_size];                          \
        break;                                                                 \
      }                                                                        \
  }}

#define ArrayList_RemoveLast(name)                                             \
  { IF_LIKELY (name##_size > 0)                                                \
    name##_size--; }

#define ArrayList_Reserve(name, n)                                             \
  { IF_LIKELY (name##_capacity < n) {                                          \
    name##_capacity = n;                                                       \
    size_t elemSize = sizeof(name##_data[0]);                                  \
    void** pData    = ((void**) &name##_data);                                 \
    *pData = MemRealloc(name##_data, name##_capacity * elemSize);              \
  }}

#define ArrayList_ZeroFill(name)                                               \
  size_t elemSize = sizeof(name##_data[0]);                                    \
  MemZero(name##_data, name##_capacity * elemSize);

#define ArrayList_Clear(name)          name##_size = 0
#define ArrayList_Get(name, index)     (name##_data[(index)])
#define ArrayList_GetData(name)        (name##_data)
#define ArrayList_GetPtr(name, index)  (name##_data + (index))
#define ArrayList_GetLast(name)        (name##_data[name##_size - 1])
#define ArrayList_GetLastPtr(name)     (name##_data + name##_size - 1)
#define ArrayList_GetCapacity(name)    (name##_capacity)
#define ArrayList_GetSize(name)        (name##_size)

#define ArrayList_Args(name)           name##_capacity, name##_size, name##_data
#define ArrayList_ArgsDef(name, T)     int32 name##_capacity, int32 name##_size, T* name##_data

#endif

#ifndef PHX_LinkedList
#define PHX_LinkedList

#define LinkedListCell(name, T)                                                \
  T** name##_prev;                                                             \
  T*  name##_next

#define LinkedListCell_Init(name)                                              \
  { name##_prev = 0; name##_next = 0; }

#define LinkedList(name, T)                                                    \
  T* name

#define LinkedList_Insert(name, elem)                                          \
  { elem->name##_prev = &name;                                                 \
    elem->name##_next =  name;                                                 \
    if (name) name->name##_prev = &elem->name##_next;                          \
    name = elem; }

#define LinkedList_Remove(name, elem)                                          \
  { *(elem->name##_prev) = elem->name##_next;                                  \
    if (elem->name##_next)                                                     \
      { (elem->name##_next)->name##_prev = elem->name##_prev; }                \
  }

#define LinkedList_ForEach(name, T, iter)                                      \
    for (T* iter = name; iter; iter = iter->name##_next)

#endif

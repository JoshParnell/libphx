#ifndef PHX_List
#define PHX_List

#define List_Free(head, T, FreeFn)                                             \
  {                                                                            \
    T* curr = (head);                                                          \
    while (curr) {                                                             \
      T* next = curr->next;                                                    \
      FreeFn(curr);                                                            \
      curr = next;                                                             \
    }                                                                          \
  }

#define List_Iterate_Begin(head, T)                                            \
  {                                                                            \
    T* curr = (head);                                                          \
    T** prev = &(head);                                                        \
    while (curr) {                                                             \
      T* next = curr->next;

#define List_Iterate_End                                                       \
      curr = next;                                                             \
    }                                                                          \
  }

#endif

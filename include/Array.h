#ifndef PHX_Array
#define PHX_Array

#define Array_Arg(x)      (x), ((int) (sizeof(x) / sizeof(*(x))))
#define Array_GetSize(x)  ((int) (sizeof(x) / sizeof(*(x))))
#define Array_Param(T)    T* arr, int len

#endif

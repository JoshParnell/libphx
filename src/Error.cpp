#include "Error.h"
#include <stdio.h>

void Error_Print (Error e) {
  printf("ERROR: ");
  if (e & Error_Stack)        printf("Stack ");
  if (e & Error_Heap)         printf("Heap ");
  if (e & Error_Buffer)       printf("Buffer ");
  if (e & Error_Path)         printf("Path ");
  if (e & Error_Index)        printf("Index ");
  if (e & Error_Vertex)       printf("Vertex ");
  if (e & Error_VertPos)      printf("Vertex Position ");
  if (e & Error_VertNorm)     printf("Vertex Normal ");
  if (e & Error_VertUV)       printf("Vertex UV ");

  if (e & Error_Input)        printf("Input ");
  if (e & Error_Intermediate) printf("Intermediate Value ");
  if (e & Error_Output)       printf("Output ");

  if (e & Error_Null)         printf("NULL");
  if (e & Error_Invalid)      printf("Invalid");
  if (e & Error_Overflow)     printf("Overflow");
  if (e & Error_Underflow)    printf("Underflow");
  if (e & Error_Empty)        printf("Empty");
  if (e & Error_NaN)          printf("NaN");
  if (e & Error_Degenerate)   printf("Degenerate");
  if (e & Error_BadCount)     printf("Incorrect Count");

  if (e == Error_None)        printf("None!");
  printf("\n");
};

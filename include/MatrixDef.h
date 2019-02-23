#ifndef PHX_MatrixDef
#define PHX_MatrixDef

#include "Common.h"

/* 0   1   2   3      00  01  02  03
   4   5   6   7      10  11  12  13
   8   9  10  11      20  21  22  23
  12  13  14  15      30  31  32  33  */

struct Matrix {
  float m[16];
};

#endif

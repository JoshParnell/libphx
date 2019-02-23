#include "Bit.h"

uint32 Bit_And32 (uint32 x, uint32 y) {
  return x & y;
}

uint32 Bit_Or32 (uint32 x, uint32 y) {
  return x | y;
}

uint32 Bit_Xor32 (uint32 x, uint32 y) {
  return x ^ y;
}

bool Bit_Has32 (uint32 x, uint32 y) {
  return (x & y) == y;
}


uint64 Bit_And64 (uint64 x, uint64 y) {
  return x & y;
}

uint64 Bit_Or64 (uint64 x, uint64 y) {
  return x | y;
}

uint64 Bit_Xor64 (uint64 x, uint64 y) {
  return x ^ y;
}

bool Bit_Has64 (uint64 x, uint64 y) {
  return (x & y) == y;
}

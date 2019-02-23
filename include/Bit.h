#ifndef PHX_Bit
#define PHX_Bit

#include "Common.h"

inline uint32 Bit_RotL32(uint32 x, int n) { return (x << n) | (x >> (32 - n)); }
inline uint32 Bit_RotR32(uint32 x, int n) { return (x << (32 - n) | (x >> n)); }

inline uint64 Bit_RotL64(uint64 x, int n) { return (x << n) | (x >> (64 - n)); }
inline uint64 Bit_RotR64(uint64 x, int n) { return (x << (64 - n) | (x >> n)); }

PHX_API uint32  Bit_And32  (uint32 x, uint32 y);
PHX_API uint32  Bit_Or32   (uint32 x, uint32 y);
PHX_API uint32  Bit_Xor32  (uint32 x, uint32 y);
PHX_API bool    Bit_Has32  (uint32 x, uint32 y);

PHX_API uint64  Bit_And64  (uint64 x, uint64 y);
PHX_API uint64  Bit_Or64   (uint64 x, uint64 y);
PHX_API uint64  Bit_Xor64  (uint64 x, uint64 y);
PHX_API bool    Bit_Has64  (uint64 x, uint64 y);

#endif

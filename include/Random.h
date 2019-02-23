#ifndef PHX_Random
#define PHX_Random

#include "Common.h"
#include "Bit.h"

inline uint64 rotl (uint64 const x, int k) {
	return (x << k) | (x >> (64 - k));
}

inline uint64 Random_SplitMix64 (uint64& state) {
	uint64 z = (state += UINT64_C(0x9E3779B97F4A7C15));
	z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
	z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
	return z ^ (z >> 31);
}

inline uint64 Random_Xoroshiro128 (uint64& state0, uint64& state1) {
	uint64 const s0 = state0;
	uint64 s1 = state1;
	uint64 const result = s0 + s1;
	s1 ^= s0;
	state0 = rotl(s0, 55) ^ s1 ^ (s1 << 14);
	state1 = rotl(s1, 36);
	return result;
}

#endif

#include "Hash.h"

/* --- Fowler–Noll–Vo ------------------------------------------------------- */

uint32 Hash_FNV32 (void const* buf, int len) {
  uchar const* curr = (uchar const*)buf;
  uchar const* end = curr + len;
  uint32 self = UINT32_C(2166136261);
  while (curr < end) {
    self ^= (uint32)*curr++;
    self *= UINT32_C(16777619);
  }
  return self;
}

uint64 Hash_FNV64 (void const* buf, int len) {
  uchar const* curr = (uchar const*)buf;
  uchar const* end = curr + len;
  uint64 self = UINT64_C(14695981039346656037);
  while (curr < end) {
    self ^= (uint64)*curr++;
    self *= UINT64_C(1099511628211);
  }
  return self;
}

uint32 Hash_FNVStr32 (cstr s) {
  uint32 self = UINT32_C(2166136261);
  while (*s) {
    self ^= (uint32)*s++;
    self *= UINT32_C(16777619);
  }
  return self;
}

uint64 Hash_FNVStr64 (cstr s) {
  uint64 self = UINT64_C(14695981039346656037);
  while (*s) {
    self ^= (uint64)*s++;
    self *= UINT64_C(1099511628211);
  }
  return self;
}

uint64 Hash_FNV64_Init () {
  return UINT64_C(14695981039346656037);
}

uint64 Hash_FNV64_Incremental (uint64 self, void const* buf, int len) {
  uchar const* curr = (uchar const*)buf;
  uchar const* end = curr + len;
  while (curr < end) {
    self ^= (uint64)*curr++;
    self *= UINT64_C(1099511628211);
  }
  return self;
}

/* --- Murmur3 -------------------------------------------------------------- */

inline static uint32 rotl32 (uint32 x, int8 r) {
  return (x << r) | (x >> (32 - r));
}

inline static uint32 fmix32 (uint32 h) {
  h ^= h >> 16; h *= 0x85EBCA6B;
  h ^= h >> 13; h *= 0xC2B2AE35;
  h ^= h >> 16;
  return h;
}

uint32 Hash_Murmur3 (void const* key, int len) {
  uint8 const* data = (uint8 const*)key;
  uint32 h1 = 0xDEADBEEF;
  uint32 const c1 = 0xCC9E2D51;
  uint32 const c2 = 0x1B873593;

  const int nblocks = len / 4;
  uint32 const* blocks = (uint32 const*)(data + nblocks*4);

  for (int i = -nblocks; i; i++) {
    uint32 k1 = blocks[i];
    k1 *= c1;
    k1 = rotl32(k1, 15);
    k1 *= c2;

    h1 ^= k1;
    h1 = rotl32(h1, 13);
    h1 = h1 * 5 + 0xE6546B64;
  }

  uint8 const* tail = (uint8 const*)(data + nblocks*4);
  uint32 k1 = 0;

  switch (len & 3) {
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1: k1 ^= tail[0];
            k1 *= c1; k1 = rotl32(k1, 15); k1 *= c2; h1 ^= k1;
  };

  h1 ^= len;
  h1 = fmix32(h1);
  return h1;
}

/* --- XXHASH64 --------------------------------------------------------------
 *   https://github.com/Cyan4973/xxHash/blob/dev/xxhash.c
 * -------------------------------------------------------------------------- */

#define XXH_rotl32(x,r) ((x << r) | (x >> (32 - r)))
#define XXH_rotl64(x,r) ((x << r) | (x >> (64 - r)))

static const uint64 PRIME64_1 = 11400714785074694791ULL;
static const uint64 PRIME64_2 = 14029467366897019727ULL;
static const uint64 PRIME64_3 =  1609587929392839161ULL;
static const uint64 PRIME64_4 =  9650029242287828579ULL;
static const uint64 PRIME64_5 =  2870177450012600261ULL;

static uint64 XXH64_round (uint64 acc, uint64 val) {
  acc += val * PRIME64_2;
  acc  = XXH_rotl64(acc, 31);
  acc *= PRIME64_1;
  return acc;
}

static uint64 XXH64_mergeRound (uint64 acc, uint64 val) {
  val  = XXH64_round(0, val);
  acc ^= val;
  acc  = acc * PRIME64_1 + PRIME64_4;
  return acc;
}

uint64 Hash_XX64 (void const* buf, int len, uint64 seed) {
  uint8 const* p = (uint8 const*)buf;
  uint8 const* end = p + len;
  uint64 hash;

  if (len >= 32) {
    uint8 const* const limit = end - 32;
    uint64 v1 = seed + PRIME64_1 + PRIME64_2;
    uint64 v2 = seed + PRIME64_2;
    uint64 v3 = seed + 0;
    uint64 v4 = seed - PRIME64_1;

    do {
      v1 = XXH64_round(v1, *(uint64 const*)p); p += 8;
      v2 = XXH64_round(v2, *(uint64 const*)p); p += 8;
      v3 = XXH64_round(v3, *(uint64 const*)p); p += 8;
      v4 = XXH64_round(v4, *(uint64 const*)p); p += 8;
    } while (p <= limit);

    hash =
      XXH_rotl64(v1,  1) +
      XXH_rotl64(v2,  7) +
      XXH_rotl64(v3, 12) +
      XXH_rotl64(v4, 18);

    hash = XXH64_mergeRound(hash, v1);
    hash = XXH64_mergeRound(hash, v2);
    hash = XXH64_mergeRound(hash, v3);
    hash = XXH64_mergeRound(hash, v4);
  } else {
    hash = seed + PRIME64_5;
  }

  hash += (uint64)len;

  while (p + 8 <= end) {
    uint64 const k1 = XXH64_round(0, *(uint64 const*)p);
    hash ^= k1;
    hash = XXH_rotl64(hash, 27) * PRIME64_1 + PRIME64_4;
    p += 8;
  }

  if (p + 4 <= end) {
    hash ^= (uint64)(*(uint32*)p) * PRIME64_1;
    hash = XXH_rotl64(hash, 23) * PRIME64_2 + PRIME64_3;
    p += 4;
  }

  while (p < end) {
    hash ^= (*p) * PRIME64_5;
    hash = XXH_rotl64(hash, 11) * PRIME64_1;
    p++;
  }

  hash ^= hash >> 33; hash *= PRIME64_2;
  hash ^= hash >> 29; hash *= PRIME64_3;
  hash ^= hash >> 32;
  return hash;
}

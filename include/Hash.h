#ifndef PHX_Hash
#define PHX_Hash

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *   Hash_FNV*     : Uses the FNV-1a variant, not the strictly-inferior FNV1
 *   Hash_FNVStr*  : FNV can operate on a null-terminated buffer without knowing
 *                   the length in advance; useful for eliminating strlen calls
 *
 *   Hash_Murmur   : In general superior to FNV in both quality and perf
 *
 *   Hash_XX64     : The best of them all. Quality similar to Murmur with perf
 *                   similar to memcpy. Why walk when you can ride? We make
 *                   a special trip, just for you.
 *
 * -------------------------------------------------------------------------- */

PHX_API uint32  Hash_FNV32     (void const* buf, int len);
PHX_API uint64  Hash_FNV64     (void const* buf, int len);
PHX_API uint32  Hash_FNVStr32  (cstr);
PHX_API uint64  Hash_FNVStr64  (cstr);

PHX_API uint64  Hash_FNV64_Init        ();
PHX_API uint64  Hash_FNV64_Incremental (uint64, void const* buf, int len);

PHX_API uint32  Hash_Murmur3   (void const* buf, int len);

PHX_API uint64  Hash_XX64      (void const* buf, int len, uint64 seed);

#endif

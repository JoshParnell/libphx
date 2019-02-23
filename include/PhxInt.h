#ifndef PHX_Int
#define PHX_Int

#include <stdint.h>

#ifndef INT8_MIN
  #define INT8_MIN    (-0x7f - 1)
  #define INT16_MIN   (-0x7fff - 1)
  #define INT32_MIN   (-0x7fffffff - 1)
  #define INT64_MIN   (-0x7fffffffffffffff - 1)
#endif

#ifndef INT8_MAX
  #define INT8_MAX    0x7f
  #define INT16_MAX   0x7fff
  #define INT32_MAX   0x7fffffff
  #define INT64_MAX   0x7fffffffffffffff
#endif

#ifndef UINT8_MAX
  #define UINT8_MAX   0xff
  #define UINT16_MAX  0xffff
  #define UINT32_MAX  0xffffffff
  #define UINT64_MAX  0xffffffffffffffff
#endif

#ifndef INT8_C
  #define INT8_C(x)    (x)
  #define INT16_C(x)   (x)
  #define INT32_C(x)   (x ## L)
  #define INT64_C(x)   (x ## LL)
#endif

#ifndef UINT8_C
  #define UINT8_C(x)   (x)
  #define UINT16_C(x)  (x)
  #define UINT32_C(x)  (x ## UL)
  #define UINT64_C(x)  (x ## ULL)
#endif

#endif

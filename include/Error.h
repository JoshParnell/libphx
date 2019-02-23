#ifndef PHX_Error
#define PHX_Error

#include "Common.h"

/* --- Error -------------------------------------------------------------------
 *
 *   A compact error status encoding.
 *
 *    high <---                                               ---> low
 *
 *           2 bytes               byte               byte
 *     [ operand location ]  [ operand type ]   [ failure type ]
 *
 *
 *   For example:
 *     0x00000101 -> received null pointer
 *     0x00080102 -> received invalid path
 *     0x00040404 -> output buffer overflow
 *     0x00000220 -> NaN detected in intermediate computation
 *     0x00200120 -> NaN detected in vertex normal of input
 *
 *   TODO : Do this in a better way, such that error information can still be
 *          concatenated/built-up from sub-functions and subjected to partial
 *          queries, but without the limitations of a bitfield.
 *
 * -------------------------------------------------------------------------- */

const Error Error_None         = 0x00000000;

const Error Error_Null         = 0x00000001;
const Error Error_Invalid      = 0x00000002;
const Error Error_Overflow     = 0x00000004;
const Error Error_Underflow    = 0x00000008;
const Error Error_Empty        = 0x00000010;
const Error Error_NaN          = 0x00000020;
const Error Error_Degenerate   = 0x00000040;
const Error Error_BadCount     = 0x00000080;

const Error Error_Input        = 0x00000100;
const Error Error_Intermediate = 0x00000200;
const Error Error_Output       = 0x00000400;

const Error Error_Stack        = 0x00010000;
const Error Error_Heap         = 0x00020000;
const Error Error_Buffer       = 0x00040000;
const Error Error_Path         = 0x00080000;
const Error Error_Index        = 0x00100000;
const Error Error_Vertex       = 0x00200000;
const Error Error_VertPos      = 0x00400000;
const Error Error_VertNorm     = 0x00800000;
const Error Error_VertUV       = 0x01000000;

PHX_API void  Error_Print  (Error);

#endif

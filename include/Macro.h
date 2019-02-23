#ifndef PHX_Macro
#define PHX_Macro

/* For the brave only. I am not proud of what I have done here. But surely, it
 * is better than template metaprogramming. */

#define  ARGC(...) _ARGC("", __VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define _ARGC(_0, _1, _2, _3, _4, _5, _6, _7, _8, N, ...) N

#define  CAT(A, B) _CAT(A, B)
#define _CAT(A, B) A ## B

#define  MACRO_STR(X) MACRO_STR1(X)
#define MACRO_STR1(X) MACRO_STR2(X)
#define MACRO_STR2(X) #X

#define  MACRO_ARG1(...) _MACRO_TAIL(1, 2, 3, 4, 5, 6, 7, __VA_ARGS__, 0)
#define  MACRO_ARG2(...) _MACRO_TAIL(1, 2, 3, 4, 5, 6, __VA_ARGS__, 0, 0)
#define  MACRO_ARG3(...) _MACRO_TAIL(1, 2, 3, 4, 5, __VA_ARGS__, 0, 0, 0)
#define  MACRO_ARG4(...) _MACRO_TAIL(1, 2, 3, 4, __VA_ARGS__, 0, 0, 0, 0)
#define  MACRO_ARG5(...) _MACRO_TAIL(1, 2, 3, __VA_ARGS__, 0, 0, 0, 0, 0)
#define  MACRO_ARG6(...) _MACRO_TAIL(1, 2, __VA_ARGS__, 0, 0, 0, 0, 0, 0)
#define  MACRO_ARG7(...) _MACRO_TAIL(1, __VA_ARGS__, 0, 0, 0, 0, 0, 0, 0)
#define  MACRO_ARG8(...) _MACRO_TAIL(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0)
#define _MACRO_TAIL(_1, _2, _3, _4, _5, _6, _7, _8, ...) _8

#define MACRO_CALL1(F) F()
#define MACRO_CALL2(F, _1) F(_1)
#define MACRO_CALL3(F, _1, _2) F(_1, _2)
#define MACRO_CALL4(F, _1, _2, _3) F(_1, _2, _3)
#define MACRO_CALL5(F, _1, _2, _3, _4) F(_1, _2, _3, _4)
#define MACRO_CALL6(F, _1, _2, _3, _4, _5) F(_1, _2, _3, _4, _5)
#define MACRO_CALL7(F, _1, _2, _3, _4, _5, _6) F(_1, _2, _3, _4, _5, _6)
#define MACRO_CALL8(F, _1, _2, _3, _4, _5, _6, _7) F(_1, _2, _3, _4, _5, _6, _7)

#define MACRO_CALL(...) CAT(MACRO_CALL, ARGC(__VA_ARGS__)(__VA_ARGS__))

#endif

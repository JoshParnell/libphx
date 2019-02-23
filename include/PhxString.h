#ifndef PHX_String
#define PHX_String

#include "Common.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

inline char*   StrAlloc     (size_t len);
inline void    StrFree      (cstr s);
inline cstr    StrAdd       (cstr a, cstr b);
inline cstr    StrAdd3      (cstr a, cstr b, cstr c);
inline cstr    StrAdd4      (cstr a, cstr b, cstr c, cstr d);
inline cstr    StrAppend    (cstr a, cstr b);
inline bool    StrBegins    (cstr s, cstr prefix);
inline int     StrCount     (cstr s, cstr sub);
inline bool    StrContains  (cstr s, cstr sub);
inline void    StrCpy       (cstr dst, cstr src);
inline cstr    StrDup       (cstr s);
inline bool    StrEqual     (cstr a, cstr b);
inline cstr    StrFind      (cstr s, cstr sub);
inline cstr    StrFormat    (cstr fmt, ...);
inline size_t  StrLen       (cstr s);
inline cstr    StrLower     (cstr s);
inline bool    StrMatch     (cstr s, cstr pattern);
inline cstr    StrReplace   (cstr s, cstr search, cstr replace);
inline int     StrSplit     (cstr s, cstr delim, cstr** out);
inline cstr    StrSub       (cstr s, cstr begin, cstr end, cstr replace);
inline cstr    StrSubStr    (cstr begin, cstr end);
inline cstr    StrUpper     (cstr s);

/* -------------------------------------------------------------------------- */

inline char* StrAlloc (size_t len) {
  return (char*)malloc(len);
}

inline void StrFree (cstr s) {
  free((void*)s);
}

inline cstr StrAdd (cstr a, cstr b) {
  char* buf = StrAlloc(StrLen(a) + StrLen(b) + 1);
  char* cur = buf;
  while (*a) *cur++ = *a++;
  while (*b) *cur++ = *b++;
  *cur = 0;
  return buf;
}

inline cstr StrAdd3 (cstr a, cstr b, cstr c) {
  char* buf = StrAlloc(StrLen(a) + StrLen(b) + StrLen(c) + 1);
  char* cur = buf;
  while (*a) *cur++ = *a++;
  while (*b) *cur++ = *b++;
  while (*c) *cur++ = *c++;
  *cur = 0;
  return buf;
}

inline cstr StrAdd4 (cstr a, cstr b, cstr c, cstr d) {
  char* buf = StrAlloc(StrLen(a) + StrLen(b) + StrLen(c) + StrLen(d) + 1);
  char* cur = buf;
  while (*a) *cur++ = *a++;
  while (*b) *cur++ = *b++;
  while (*c) *cur++ = *c++;
  while (*d) *cur++ = *d++;
  *cur = 0;
  return buf;
}

/* NOTE: Consumes a. */
inline cstr StrAppend (cstr a, cstr b) {
  cstr c = StrAdd(a, b);
  StrFree(a);
  return c;
}

inline bool StrBegins (cstr s, cstr prefix) {
  for (; ; s++, prefix++)
    if (!*prefix)
      return true;
    else if (*s != *prefix)
      return false;
}

inline int StrCount (cstr s, cstr sub) {
  size_t n = StrLen(sub);
  int count;
  for (count = 0; (s = strstr(s, sub)); ++count) s += n;
  return count;
}

inline bool StrContains (cstr s, cstr sub) {
  return strstr(s, sub) != 0;
}

inline void StrCpy (cstr pDst, cstr src) {
  char* dst = (char*)pDst;
  while (*src)
    *dst++ = *src++;
  *dst = 0;
}

inline cstr StrDup (cstr s) {
  if (!s) return 0;
  size_t len = StrLen(s) + 1;
  char* buf = StrAlloc(len);
  memcpy(buf, s, len);
  return buf;
}

inline bool StrEqual (cstr a, cstr b) {
  return strcmp(a, b) == 0;
}

inline cstr StrFind (cstr s, cstr sub) {
  return strstr(s, sub);
}

inline cstr StrFormat (cstr fmt, ...) {
  va_list args;
  va_start(args, fmt);
  size_t len = vsnprintf(0, 0, fmt, args) + 1;
  va_end(args);
  char* buf = StrAlloc(len);
  va_start(args, fmt);
  vsnprintf(buf, len, fmt, args);
  va_end(args);
  return buf;
}

inline size_t StrLen (cstr s) {
  if (!s) return 0;
  cstr begin = s;
  while (*s)
    ++s;
  return s - begin;
}

inline cstr StrLower (cstr ps) {
  char* s = (char*)ps;
  for (size_t i = 0, n = StrLen(s); i < n; ++i)
    if (isupper(s[i]))
      s[i] |= 0x20;
  return s;
}

inline bool StrMatch (cstr s, cstr p) {
  /* NOTE: An empty p will only match an empty string.
   *       Wildcards can match zero characters.
   *       Wildcards are non-greedy, but will grow as necessary.
   *       There is no way to match a literal '*' or '?'.
   *       If the parameters are swapped, this will fail somewhat silently.
   *
   *       This is not guaranteed to work for complex, multi-asterisk patterns.
   *       I believe it will produce false negatives in some (rare) cases.
   *       https://leetcode.com/problems/wildcard-matching/#/solutions (first one)
   *       http://yucoding.blogspot.com/2013/02/leetcode-question-123-wildcard-matching.html
   *
   *       If this ends up being insufficient, I believe a non-deterministic finite
   *       automata is the next step up.
   *       https://swtch.com/~rsc/regexp/regexp1.html */

  if (!s || !p) return false;

  const char* star = 0;
  const char* ss   = s;

  while (*s) {
    if ((*p == '?') || (*p == *s)) { s++; p++;               continue; }
    if (*p == '*')                 { star = p++; ss = s;     continue; }
    if (star)                      { p = star + 1; s = ++ss; continue; }

    return false;
  }

  while (*p == '*') p++;
  return !*p;
}

inline cstr StrReplace (cstr s, cstr search, cstr replace) {
  char *result, *ins, *tmp;
  size_t len_search, len_replace, len_front, count;

  if (!s || !search) return 0;
  len_search = StrLen(search);
  if (len_search == 0) return 0;
  if (!replace) replace = "";
  len_replace = StrLen(replace);

  ins = (char*)s;
  for (count = 0; (tmp = strstr(ins, search)); ++count)
    ins = tmp + len_search;
  tmp = result = StrAlloc(StrLen(s) + (len_replace - len_search) * count + 1);

  while (count--) {
    ins = (char*)strstr(s, search);
    len_front = ins - s;
    tmp = strncpy(tmp, s, len_front) + len_front;
    tmp = strcpy(tmp, replace) + len_replace;
    s += len_front + len_search;
  }
  strcpy(tmp, s);
  return result;
}

inline int StrSplit (cstr s, cstr delim, cstr** out) {
  size_t len_orig = StrLen(s);
  int count = StrCount(s, delim);
  if (count == 0)
    return 0;

  *out = (cstr*)StrAlloc(sizeof(cstr) * (count + 1));
  size_t len_delim = StrLen(delim);
  cstr next;
  size_t i;
  for (i = 0; (next = strstr(s, delim)); ++i) {
    size_t sz = next - s;
    (*out)[i] = StrAlloc(sz + 1);
    memcpy((char*)(*out)[i], s, sz);
    ((char*)((*out)[i]))[sz] = 0;
    s = next + len_delim;
  }

  size_t remaining = len_orig - i;
  (*out)[count] = StrAlloc(remaining + 1);
  memcpy((char*)(*out)[count], s, remaining);
  ((char*)((*out)[count]))[remaining] = 0;
  return count + 1;
}

inline cstr StrSub (cstr s, cstr begin, cstr end, cstr replace) {
  size_t len = StrLen(s) + StrLen(replace) + begin - end;
  char* result = StrAlloc(len + 1);
  char* pResult = result;
  while (s != begin)
    *pResult++ = *s++;
  while (*replace)
    *pResult++ = *replace++;
  while (*end)
    *pResult++ = *end++;
  *pResult = 0;
  return result;
}

inline cstr StrSubStr (cstr begin, cstr end) {
  size_t len = (end - begin);
  char* result = StrAlloc(len + 1);
  char* pResult = result;
  while (begin != end)
    *pResult++ = *begin++;
  result[len] = 0;
  return result;
}

inline cstr StrUpper (cstr ps) {
  char* s = (char*)ps;
  for (size_t i = 0, n = StrLen(s); i < n; ++i)
    if (islower(s[i]))
      s[i] &= ~0x20;
  return s;
}

#endif

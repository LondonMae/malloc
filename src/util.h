#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>

#ifdef DEBUG

#include <assert.h>

#define DEBUG_PRINT(format, args...)                                           \
  fprintf(stderr, "D %s:%d:%s(): " format, __FILE__, __LINE__, __func__, ##args)

#else

#define DEBUG_PRINT(fmt, args...) // no-op

// Warning -- this requires malloc to be functional, since fprintf calls malloc.
// If this allocator is being used as the implementation of malloc, this assert
// may call lynx_malloc.
#define assert(expr)                                                           \
  if (!(expr)) {                                                               \
    fprintf(stderr, "Error %s:%d:%s: %s\n", __FILE__, __LINE__, __func__,      \
            "" #expr "");                                                      \
    __asm__ __volatile__("int $3");                                            \
  }

#endif

#endif

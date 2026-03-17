#pragma once
#include <stdio.h>

#ifdef NDEBUG
/**
 * Assert that a condition is true. If NDEBUG is defined, it will just dereference NULL.
 */
#define assert(x) ((void)0)
#else
/**
 * Assert that a condition is true.
 * @param x condition
 */
#define assert(x)                                                               \
  do {                                                                          \
    if (!(x)) {                                                                 \
      fprintf(stderr, "Assert '%s' failed at %s:%d\n", #x, __FILE__, __LINE__); \
      *(volatile int*)0 = 0;                                                    \
    }                                                                           \
  } while (0)
#endif

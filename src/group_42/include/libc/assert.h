#pragma once
#include <stdio.h>

#ifdef NDEBUG
#  define assert(x) ((void)0)
#else
#  define assert(x) \
do { \
if (!(x)) { \
fprintf(stderr, "Assert '%s' failed at %s:%d\n", #x, __FILE__, __LINE__); \
*(volatile int*)0 = 0; \
} \
} while (0)
#endif
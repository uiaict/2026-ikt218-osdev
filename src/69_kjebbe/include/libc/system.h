#pragma once

#include <libc/stdio.h>

/* Halt the kernel with a message. Used by malloc.c on OOM. */
static inline void panic(const char* msg)
{
    printf("KERNEL PANIC: ");
    printf(msg);
    for (;;) {}
}

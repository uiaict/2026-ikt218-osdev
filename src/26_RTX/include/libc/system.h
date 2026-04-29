#ifndef SYSTEM_H
#define SYSTEM_H

#include <libc/stdio.h>

static inline void panic(const char *msg)
{
    printf("PANIC: %s\n", msg);
    for (;;) __asm__ volatile ("hlt");
}

#endif

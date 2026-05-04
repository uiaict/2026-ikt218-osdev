#pragma once

#include <libc/stdio.h>

static inline __attribute__((noreturn)) void panic(const char *msg)
{
    printf("= KERNEL PANIC! =");
    printf(msg);

    /* Disable interrupts and halt system*/
    for (;;) {
        asm volatile ("cli; hlt");
    }
} 

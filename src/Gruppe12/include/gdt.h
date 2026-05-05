#ifndef GDT_H
#define GDT_H
#include "libc/stdint.h"  // for faste heltalltyper som uint32_t osv.


// Initialiserer og laster GDT inn i CPU-en
void gdt_init(void);

#endif
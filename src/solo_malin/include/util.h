#ifndef UTIL_H
#define UTIL_H

#include <libc/stdint.h>

//void memset(void *dest, char val, uint32_t count);
void outPortB(uint16_t port, uint8_t value);
void panic(const char* message);

#endif

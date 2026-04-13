#ifndef _STDINT_H
#define _STDINT_H

// Custom implementation of stdint to bypass GCC include_next bug in freestanding mode
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

#endif
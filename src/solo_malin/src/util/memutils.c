#include "memory.h"
#include <libc/stdint.h>

// ==============================
// Memory utility functions
//
// Basic implementations of memcpy,
// memset16, and memset.
// ==============================

// Copy 'count' bytes from src to dest
void* memcpy(void* dest, const void* src, size_t count){

    uint8_t* dst8 = (uint8_t*)dest;
    const uint8_t* src8 = (const uint8_t*)src;

    // Copy byte by byte
    while (count--){

        *dst8++ = *src8++;
    }

    return dest;
}

// Set 'num' 16-bit values at ptr to 'value'
void* memset16(void* ptr, uint16_t value, size_t num)
{
    uint16_t* p = (uint16_t*)ptr;

    while (num--) {
        *p++ = value;
    }

    return ptr;
}

// Set 'num' bytes at ptr to the given byte value
void* memset(void* ptr, int value, size_t num){
    
    unsigned char* p = (unsigned char*)ptr;

    while (num--){
        *p++ = (unsigned char)value;
    }

    return ptr;
}

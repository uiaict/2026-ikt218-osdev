#include "memory.h"
#include <libc/stdint.h>

void* memcpy(void* dest, const void* src, size_t count){

    uint8_t* dst8 = (uint8_t*)dest;
    const uint8_t* src8 = (const uint8_t*)src;

    //if (count & 1){
    //
    //    dst8[0] = src8[0];
    //    dst8++:
    //    src8++;
    //    count--;
    //}

    //count /= 2;
    while (count--){

        *dst8++ = *src8++;
        //dst8[0] = src8[0];
        //dst8[1] = src8[1];
        //dst8 += 2;
        //src8 += 2;
    }

    return dest;
}

void* memset16(void* ptr, uint16_t value, size_t num)
{
    uint16_t* p = (uint16_t*)ptr;

    while (num--) {
        *p++ = value;
    }

    return ptr;
}

void* memset(void* ptr, int value, size_t num){
    
    unsigned char* p = (unsigned char*)ptr;

    while (num--){
        *p++ = (unsigned char)value;
    }

    return ptr;
}

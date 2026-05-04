#include "libc/string.h"

uint32 strlen(const char* str) {
    uint32 len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}
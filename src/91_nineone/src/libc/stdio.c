#include "libc/stdio.h"
#include "../terminal.h"

#define print terminal_write


void print_uint(uint32_t n, uint8_t color, int x, int y) {
    char buffer[11];
    int i = 0;

    if (n == 0) {
        print("0", color, x, y);
        return;
    }

    while (n > 0) {
        buffer[i++] = '0' + (n % 10);
        n /= 10;
    }

    char out[12];
    int j = 0;

    while (i > 0) {
        out[j++] = buffer[--i];
    }

    out[j] = '\0';

    print(out, color, x, y);
}
#include "libc/stdio.h"
#include "terminal.h"
#include "libc/stdarg.h"
#include "../colors.h"

#define print terminal_write


void print_uint(uint32 n, uint8 color, int x, int y) {
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


int printf(const char* __restrict__ format, ...) 
{
    va_list args;
    va_start(args, format);

    while(*format)
    {
        if(*format == '%') 
        {
            format++;
            
            if(*format == 'd') 
            {
                int value = va_arg(args, int);

            }
        }
        else 
        {
            terminal_putchar(*format, COLOR(WHITE, BLACK));
        }
    }


    va_end(args);


    return length;
}



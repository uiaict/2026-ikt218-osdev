#include "libc/stdio.h"
#include "terminal.h"
#include "libc/stdarg.h"
#include "colors.h"

#define print terminal_write

static void stdio_putchar(char c, uint8 color, int* x, int* y);
static int vprintf_color(uint8 color, const char* format, va_list args);
static int print_hex(uint32 n, uint8 color, int* x, int* y);
int print_uint2(uint32 n, uint8 color, int* x, int* y);
static int print_int(int n, uint8 color, int* x, int* y);


static int currentRowNumber = 1;
 
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

static int print_hex(uint32 n, uint8 color, int* x, int* y)
{
    char buffer[9];
    int i = 0;
    int count = 0;

    if (n == 0) {
        stdio_putchar('0', color, x, y);
        return 1;
    }

    while (n > 0) {
        uint32 digit = n % 16;

        if (digit < 10)
            buffer[i++] = '0' + digit;
        else
            buffer[i++] = 'a' + (digit - 10);

        n /= 16;
    }

    while (i > 0) {
        stdio_putchar(buffer[--i], color, x, y);
        count++;
    }

    return count;
}

 // heart-aching
int printf(const char* __restrict__ format, ...) 
{
    uint8 color = COLOR(WHITE, BLACK); //Default color
    
    va_list args;
    va_start(args, format);

    int length = vprintf_color(color, format, args); 

    va_end(args);

    return length; 
}


int printf_color(uint8 color, const char* __restrict__ format, ...)
{   
    va_list args;
    va_start(args, format);

    int length = vprintf_color(color, format, args); 

    va_end(args);

    return length; 
}
    

int print_uint2(uint32 n, uint8 color, int* x, int* y) 
{
    char buffer[11];
    int i = 0;
    int count = 0;

    if (n == 0) {
        stdio_putchar('0', color, x, y);
        return 1;
    }

    while (n > 0) {
        buffer[i++] = '0' + (n % 10);
        n /= 10;
    }

    while (i > 0) {
        stdio_putchar(buffer[--i], color, x, y);
        count++;
    }

    return count;
}

static int print_int(int n, uint8 color, int* x, int* y)
{
    int count = 0;

    if (n < 0)
    {
        stdio_putchar('-', color, x, y);
        count++;
        n = -n;
    }

    count += print_uint2((uint32)n, color, x, y);
    return count;
}

static void stdio_putchar(char c, uint8 color, int* x, int* y) 
{
    terminal_putchar(c, color, *x, *y);

    (*x)++;

    if(*x >= 79) 
    {
        *x= 1;
        (*y)++;
    }
}

static int vprintf_color(uint8 color, const char* format, va_list args) 
{
    int length = 0;
    int currentX = 1;

    while(*format)
    {
        if(*format == '%') 
        {
            format++;
            
            if(*format == 'u') 
            {
                int value = va_arg(args, int);
                length += print_uint2(value, color, &currentX, &currentRowNumber);
            }
            else if(*format == 'x') 
            {
                uint32 value = va_arg(args, uint32);
                length += print_hex(value, color, &currentX, &currentRowNumber);
            }
            else if(*format == 'd') 
            {
                int value = va_arg(args, int);
                length += print_int(value, color, &currentX, &currentRowNumber);
            }
        }
        else 
        {
            stdio_putchar(*format, color, &currentX, &currentRowNumber);
            length++;
        }
        
        format++;
    }

    currentRowNumber++;

    return length;
}

void resetRowNumber() 
{
    currentRowNumber = 1;
}





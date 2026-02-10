#include "libc/stdio.h"
#include "libc/stdbool.h"
#include "libc/stdint.h"
#include "libc/stdarg.h"
#include "vga_terminal.h"
#include "libc/stddef.h"


struct _IO_FILE {
    int fd;
    int flags; 
    unsigned char *buf;
    unsigned char *rpos; // Read position
    unsigned char *wpos; // Write position
    // maybe lock or mutex here
};

static unsigned char stdin_buf[BUFSIZ];
static unsigned char stdout_buf[BUFSIZ];


static struct _IO_FILE __stdin = {
    .fd = 0,
    .flags = _IO_READ,
    .buf = stdin_buf,

};

static struct _IO_FILE __stdout = {
    .fd = 1,
    .flags = _IO_WRITE,
    .buf = stdout_buf,
    
};

static struct _IO_FILE __stderr = {
    .fd = 2,
    .flags = _IO_WRITE | _IO_UNBUFFERED,
    .buf = NULL,
    
};

FILE *const stdin = &__stdin;
FILE *const stdout = &__stdout;
FILE *const stderr = &__stderr;


/**
 * Prints a char to terminal
 * @param ic char to print
 * @return char printed
 */
int putchar(int ic){

    // TODO: Buffer through stdout->buf
    vga_terminal_putchar((char)ic);
    return ic;
}

/**
 * Writes a char to a stream
 * @param c char to write
 * @param stream stream to write into
 * @return char written
 */
int fputc(int c, FILE *stream) {
    if (stream == stdout ) {
        return putchar(c);
    }
    if (stream == stderr) {
        vga_terminal_setcolor(vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK));
        return putchar(c);
    }
    // TODO: handle other streams
    return c;
}
int fprintf( FILE *restrict stream, const char *restrict format, ... ) {
    va_list args;
    va_start(args, format);
    int ret = vfprintf(stream, format, args);
    va_end(args);
    return ret;
}
int printf(const char* restrict format, ...){
    va_list args;
    va_start(args, format);

    int ret = vfprintf(stdout, format, args);

    va_end(args);
    return ret;
}
static void print_dec(long value, FILE *stream, int *count) {
    if (value < 0) {
        fputc('-', stream); 
        (*count)++;
        value = -value;
    }
    
    if (value / 10) {
        print_dec(value / 10, stream, count);
    }
    
    fputc((value % 10) + '0', stream);
    (*count)++;
}

static void print_hex(unsigned long value, FILE *stream, int *count) {
    if (value / 16) {
        print_hex(value / 16, stream, count);
    }
    
    int digit = value % 16;
    char c = digit < 10 ? digit + '0' : digit - 10 + 'a';
    fputc(c, stream);
    (*count)++;
}

int vfprintf(FILE *stream, const char *format, va_list arg) {
    int printed_chars = 0;
    
    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] != '%') {
            fputc(format[i], stream);
            printed_chars++;
            continue;
        }

        i++; // skip '%'
        
        // TODO: Floating point
        switch (format[i]) {
            case 'c': {
                char c = (char)va_arg(arg, int);
                fputc(c, stream);
                printed_chars++;
                break;
            }
            case 's': {
                const char *s = va_arg(arg, const char *);
                // TODO: Should we print emtpy or null..
                if (!s) s = "(null)"; 
                while (*s) {
                    fputc(*s++, stream);
                    printed_chars++;
                }
                break;
            }
            case 'd': 
            case 'i': {
                int d = va_arg(arg, int);
                print_dec(d, stream, &printed_chars);
                break;
            }
            case 'x': 
            case 'p': {
                unsigned long x;
                if (format[i] == 'p') {
                     x = (uintptr_t) va_arg(arg, void*);
                     fputc('0', stream); 
                     fputc('x', stream);
                     printed_chars += 2;
                } else {
                     x = va_arg(arg, unsigned int);
                }
                print_hex(x, stream, &printed_chars);
                break;
            }
            case '%': {
                fputc('%', stream);
                printed_chars++;
                break;
            }
            default:
                // just literally print it, if we dont know it
                fputc('%', stream);
                fputc(format[i], stream);
                printed_chars += 2;
                break;
        }
    }
    return printed_chars;
}
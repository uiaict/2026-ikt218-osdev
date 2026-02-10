#pragma once
#include "libc/stdbool.h"
#include "libc/stdint.h"
#include "libc/stdarg.h"

#define BUFSIZ 1024
typedef struct _IO_FILE FILE;

#define stdin  (stdin)  // file descriptor 0
#define stdout (stdout) // file descriptor 1
#define stderr (stderr) // file decriptor 2

#define _IO_READ 1
#define _IO_WRITE 2
#define _IO_UNBUFFERED 4
#define _IO_EOF_SEEN 8
#define _IO_ERR_SEEN 16

#define EOF (-1)

extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;


int fputc(int c, FILE *stream);
int putchar(int ic);
int printf(const char* restrict format, ...);
int fprintf( FILE* restrict stream, const char* restrict format, ... );
int vfprintf(FILE *stream, const char *format, va_list arg);
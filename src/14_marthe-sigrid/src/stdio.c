#include "../include/libc/stdio.h"
#include "../include/libc/stdarg.h"
#include "../include/libc/stddef.h"
#include "../include/libc/stdint.h"

// the screen is just memory at 0xB8000. it's an 80x25 grid where each
// slot is 2 bytes: the character + a colour byte stuck on top of it.
#define COLS 80
#define ROWS 25
#define CELLS (COLS * ROWS)

static volatile uint16_t *const vga = (uint16_t *)0xB8000;

// instead of tracking a row and a column, we just keep one number that
// counts cells from the top-left. less stuff to keep in sync.
static unsigned cursor = 0;
static uint8_t  default_attr = 0x07;  // boring grey-on-black

// glue a character and its colour together into the 16-bit format the VGA wants.
static inline uint16_t cell_of(char c, uint8_t attr) {
    return ((uint16_t)attr << 8) | (uint8_t)c;
}

// cursor went past the bottom — slide everything up one row and start
// over on the now-empty bottom line.
static void roll_up(void) {
    // copy each row onto the row above it
    for (unsigned i = COLS; i < CELLS; i++) {
        vga[i - COLS] = vga[i];
    }
    // wipe the bottom row clean
    uint16_t blank = cell_of(' ', default_attr);
    for (unsigned i = CELLS - COLS; i < CELLS; i++) {
        vga[i] = blank;
    }
    cursor = CELLS - COLS;  // park at the start of that empty row
}

// drop one character onto the screen. '\n' just jumps to the next row,
// everything else gets written and we shuffle the cursor forward.
static void emit(char c, uint8_t attr) {
    if (c == '\n') {
        // round the cursor up to the start of the next row
        cursor = (cursor / COLS + 1) * COLS;
    } else {
        vga[cursor++] = cell_of(c, attr);
    }
    if (cursor >= CELLS) {
        roll_up();
    }
}

// just spam emit() down a string until the null terminator.
static void emit_str(const char *s, uint8_t attr) {
    while (*s) emit(*s++, attr);
}

// blank the whole screen and send the cursor back home.
void terminal_clear(void) {
    uint16_t blank = cell_of(' ', default_attr);
    for (unsigned i = 0; i < CELLS; i++) {
        vga[i] = blank;
    }
    cursor = 0;
}

// no format specifiers, just print this string in the colour you picked.
// used by the menu so we can paint each line in a different colour.
void printf_color(const char *str, uint8_t color) {
    emit_str(str, color);
}

// turn an unsigned integer into a string, but built from the *back* of the
// buffer instead of the front. that way the digits come out in the right
// order on the first try — no separate reverse-print step needed.
// returns a pointer to wherever the first digit ended up.
static const char *u_to_str(uint32_t n, unsigned base, char *buf, size_t cap) {
    static const char digits[] = "0123456789abcdef";
    char *p = buf + cap;
    *--p = '\0';                // null terminator at the very end
    if (n == 0) {
        *--p = '0';             // 0 doesn't enter the loop, handle it explicitly
        return p;
    }
    while (n > 0) {
        *--p = digits[n % base]; // last digit first, then walk leftwards
        n /= base;
    }
    return p;
}

// %d handler. negatives print a '-' first and then the magnitude. the
// goofy `(uint32_t)(-(v+1))+1u` is just so INT_MIN doesn't blow up — you
// can't negate INT_MIN as a signed int.
static void emit_int(int v, uint8_t attr) {
    char buf[16];
    uint32_t mag = (v < 0) ? (uint32_t)(-(v + 1)) + 1u : (uint32_t)v;
    if (v < 0) emit('-', attr);
    emit_str(u_to_str(mag, 10, buf, sizeof(buf)), attr);
}

// %x handler. always prefix with "0x" so it's obvious what base it is.
static void emit_hex(uint32_t v, uint8_t attr) {
    char buf[16];
    emit_str("0x", attr);
    emit_str(u_to_str(v, 16, buf, sizeof(buf)), attr);
}

// the actual printf brains. walk the format string char by char, and
// when we hit a %, peek at what comes after and call the right handler.
// anything weird (like %z) just gets printed literally so we can spot bugs.
static void format_into(const char *fmt, va_list args, uint8_t attr) {
    while (*fmt) {
        if (*fmt != '%' || fmt[1] == '\0') {
            emit(*fmt++, attr);
            continue;
        }
        char spec = *++fmt;
        fmt++;
        if      (spec == 's') emit_str(va_arg(args, const char *), attr);
        else if (spec == 'd') emit_int(va_arg(args, int), attr);
        else if (spec == 'x') emit_hex(va_arg(args, uint32_t), attr);
        else if (spec == 'c') emit((char)va_arg(args, int), attr);
        else if (spec == '%') emit('%', attr);
        else { emit('%', attr); emit(spec, attr); }
    }
}

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    format_into(fmt, args, default_attr);
    va_end(args);
    return 0;
}

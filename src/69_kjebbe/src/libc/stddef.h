typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)
#include "./stdint.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

static uint16_t *terminal_buffer = (uint16_t *)VGA_MEMORY;
static int terminal_row = 0;
static int terminal_col = 0;
static uint8_t terminal_color = 0x07; // light grey on black

void terminal_putchar(char c) {
  if (c == '\n') {
    terminal_col = 0;
    terminal_row++;
    return;
  }

  int index = terminal_row * VGA_WIDTH + terminal_col;
  terminal_buffer[index] = (terminal_color << 8) | c;
  terminal_col++;

  if (terminal_col >= VGA_WIDTH) {
    terminal_col = 0;
    terminal_row++;
  }
}

void terminal_write(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    terminal_putchar(str[i]);
  }
}
// Helper: print a decimal integer
void print_int(int value) {
  if (value < 0) {
    terminal_putchar('-');
    value = -value;
  }

  char buf[12];
  int i = 0;

  if (value == 0) {
    terminal_putchar('0');
    return;
  }

  while (value > 0) {
    buf[i++] = '0' + (value % 10);
    value /= 10;
  }

  // digits are in reverse order, so print backwards
  while (i > 0) {
    terminal_putchar(buf[--i]);
  }
}

// Helper: print a hex integer
void print_hex(uint32_t value) {
  char hex_chars[] = "0123456789ABCDEF";
  terminal_write("0x");

  char buf[8];
  int i = 0;

  if (value == 0) {
    terminal_putchar('0');
    return;
  }

  while (value > 0) {
    buf[i++] = hex_chars[value & 0xF];
    value >>= 4;
  }

  while (i > 0) {
    terminal_putchar(buf[--i]);
  }
}

void printf(const char *format, ...) {
  va_list args;
  va_start(args, format);

  for (int i = 0; format[i] != '\0'; i++) {
    if (format[i] == '%' && format[i + 1] != '\0') {
      i++;
      switch (format[i]) {
      case 's':
        terminal_write(va_arg(args, const char *));
        break;
      case 'd':
        print_int(va_arg(args, int));
        break;
      case 'x':
        print_hex(va_arg(args, uint32_t));
        break;
      case 'c':
        terminal_putchar((char)va_arg(args, int));
        break;
      case '%':
        terminal_putchar('%');
        break;
      }
    } else {
      terminal_putchar(format[i]);
    }
  }

  va_end(args);
}

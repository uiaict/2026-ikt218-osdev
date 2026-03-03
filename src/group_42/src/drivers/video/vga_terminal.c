#include "drivers/video/vga_terminal.h"

#include "arch/i386/cpu/ports.h"
#include "libc/stdint.h"
#include "libc/string.h"

/// Packs colour and char together for printing
/// @param uc character
/// @param color colour
/// @return packed unsigned 16 bit int, ready to be printed
static uint16_t vga_entry(char uc, uint8_t color) {
  return (uint16_t)uc | (uint16_t)color << 8;
}


size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;


void vga_terminal_initialise(void) {
  vga_clear_screen();
}

void vga_clear_screen(void) {
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

  // Fill entire VGA with blank entries with correct colours
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

void vga_terminal_setcolor(uint8_t color) {
  terminal_color = color;
}


void vga_terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

// NOTE:
#define HTAB_WIDTH 4
#define VTAB_WIDTH 4

void vga_terminal_putchar(char c) {
  switch (c) {
    case '\n': { // newline

      // clear cell, for marker
      const size_t index = terminal_row * VGA_WIDTH + terminal_column;
      terminal_buffer[index] = vga_entry(' ', terminal_color);

      terminal_column = 0;
      terminal_row++;
      if (terminal_row == VGA_HEIGHT)
        vga_terminal_scroll();
      return;
    }
    case '\t': { // horizontal tab
      // 4 - (1%4) = 4-1 = +3
      // 4 - (2%4) = +2
      // 4 - (3%4) = +1
      // 4 - (4%4) = 0 -> 4
      size_t offset = HTAB_WIDTH - (terminal_column % HTAB_WIDTH);
      terminal_column += offset ? offset : HTAB_WIDTH;
      return;
    }
    case '\r': { // carriage return
      terminal_column = 0;
      return;
    }
    case '\f': { // form feed
      vga_clear_screen();
      return;
    }
    case '\b': { // backspace
      if (terminal_column != 0)
        terminal_column--;
      return;
    }
    case '\v': { // vertical tab
      // NOTE: see '\t' handling
      size_t offset = VTAB_WIDTH - (terminal_row % VTAB_WIDTH);
      terminal_row += offset ? offset : HTAB_WIDTH;
      return;
    }
    case '\a': {
      // ignored for now
      // TODO: add audible beep when we have sound working nicely
      return;
    }
    default:
      break;
  }


  const size_t index = terminal_row * VGA_WIDTH + terminal_column;
  terminal_buffer[index] = vga_entry(c, terminal_color);

  terminal_column++;

  if (terminal_column == VGA_WIDTH) {
    terminal_column = 0;
    terminal_row++;

    if (terminal_row == VGA_HEIGHT) {
      vga_terminal_scroll();
    }
  }
}
void vga_terminal_write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++)
    vga_terminal_putchar(data[i]);
}
void vga_terminal_writestring(const char* data) {
  vga_terminal_write(data, strlen(data));
}

void vga_terminal_scroll(void) {
  for (size_t y = 1; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      size_t dest_index = (y - 1) * VGA_WIDTH + x;
      size_t src_index = y * VGA_WIDTH + x;
      terminal_buffer[dest_index] = terminal_buffer[src_index];
    }
  }

  size_t last_row_start = (VGA_HEIGHT - 1) * VGA_WIDTH;
  for (size_t x = 0; x < VGA_WIDTH; x++) {
    terminal_buffer[last_row_start + x] = vga_entry(' ', terminal_color);
  }
  terminal_row = VGA_HEIGHT - 1;
}

uint16_t vga_terminal_get_entry_at(size_t x, size_t y) {
  return terminal_buffer[y * VGA_WIDTH + x];
}

uint8_t vga_terminal_get_color(void) {
  return terminal_color;
}

void vga_get_cursor_position(size_t* x, size_t* y) {
  *x = terminal_column;
  *y = terminal_row;
}

#define VGA_CRTC_ADDRESS_PORT_COLOR 0x3D4
#define VGA_CRTC_DATA_PORT_COLOR 0x3D5
#define VGA_CRTC_CURSOR_START_INDEX 0x0A
#define VGA_CRTC_CURSOR_DISABLE_MASK 0x20

void vga_disable_cursor() {
  port_byte_out(VGA_CRTC_ADDRESS_PORT_COLOR, VGA_CRTC_CURSOR_START_INDEX);
  port_byte_out(VGA_CRTC_DATA_PORT_COLOR, VGA_CRTC_CURSOR_DISABLE_MASK);
}

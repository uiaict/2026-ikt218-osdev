#include "drivers/video/vga_terminal.h"

#include <stdbool.h>

#include "arch/i386/cpu/ports.h"
#include "libc/stdint.h"
#include "libc/string.h"

#define SCROLLBACK_LINES 1000

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

static uint16_t scrollback[SCROLLBACK_LINES * VGA_WIDTH];
static int scrollback_head = 0;
static int scrollback_tail = 0;
static int scrollback_count = 0;

static bool scroll_mode = false;
static int scroll_offset = 0;

static uint16_t live_screen[VGA_HEIGHT * VGA_WIDTH];

static void vga_terminal_save_line_at_row(size_t row);
static void vga_terminal_save_live_screen(void);
static void vga_terminal_restore_live_screen(void);
static void vga_terminal_update_display(void);

static uint16_t vga_entry(char uc, uint8_t color) {
  return (uint16_t)uc | (uint16_t)color << 8;
}

void vga_terminal_initialise(void) {
  vga_clear_screen();
  scrollback_head = 0;
  scrollback_tail = 0;
  scrollback_count = 0;
  scroll_mode = false;
  scroll_offset = 0;
}

void vga_clear_screen(void) {
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
      live_screen[index] = vga_entry(' ', terminal_color);
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

#define HTAB_WIDTH 4
#define VTAB_WIDTH 4

void vga_terminal_putchar(char c) {
  if (scroll_mode) {
    scroll_mode = false;
    scroll_offset = 0;
    vga_terminal_restore_live_screen();
  }

  switch (c) {
    case '\n': {
      vga_terminal_save_line_at_row(terminal_row);
      
      const size_t index = terminal_row * VGA_WIDTH + terminal_column;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
      live_screen[index] = vga_entry(' ', terminal_color);

      terminal_column = 0;
      if (terminal_row < VGA_HEIGHT - 1) {
        terminal_row++;
      } else {
        for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
          for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
            live_screen[y * VGA_WIDTH + x] = live_screen[(y + 1) * VGA_WIDTH + x];
          }
        }
        size_t last_row = (VGA_HEIGHT - 1) * VGA_WIDTH;
        for (size_t x = 0; x < VGA_WIDTH; x++) {
          terminal_buffer[last_row + x] = vga_entry(' ', terminal_color);
          live_screen[last_row + x] = vga_entry(' ', terminal_color);
        }
      }
      return;
    }
    case '\t': {
      size_t offset = HTAB_WIDTH - (terminal_column % HTAB_WIDTH);
      terminal_column += offset ? offset : HTAB_WIDTH;
      return;
    }
    case '\r': {
      terminal_column = 0;
      return;
    }
    case '\f': {
      vga_clear_screen();
      return;
    }
    case '\b': {
      if (terminal_column != 0)
        terminal_column--;
      return;
    }
    case '\v': {
      size_t offset = VTAB_WIDTH - (terminal_row % VTAB_WIDTH);
      terminal_row += offset ? offset : VTAB_WIDTH;
      return;
    }
    case '\a': {
      return;
    }
    default:
      break;
  }

  const size_t index = terminal_row * VGA_WIDTH + terminal_column;
  terminal_buffer[index] = vga_entry(c, terminal_color);
  live_screen[index] = vga_entry(c, terminal_color);

  terminal_column++;
  if (terminal_column >= VGA_WIDTH) {
    terminal_column = 0;
    if (terminal_row < VGA_HEIGHT - 1) {
      terminal_row++;
    } else {
      vga_terminal_save_line_at_row(0);
      for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
          terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
          live_screen[y * VGA_WIDTH + x] = live_screen[(y + 1) * VGA_WIDTH + x];
        }
      }
      size_t last_row = (VGA_HEIGHT - 1) * VGA_WIDTH;
      for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[last_row + x] = vga_entry(' ', terminal_color);
        live_screen[last_row + x] = vga_entry(' ', terminal_color);
      }
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

static void vga_terminal_save_line_at_row(size_t row) {
  int dest_idx = scrollback_head * VGA_WIDTH;

  for (size_t x = 0; x < VGA_WIDTH; x++) {
    scrollback[dest_idx + x] = live_screen[row * VGA_WIDTH + x];
  }

  scrollback_head = (scrollback_head + 1) % SCROLLBACK_LINES;
  
  if (scrollback_count < SCROLLBACK_LINES) {
    scrollback_count++;
  } else {
    scrollback_tail = (scrollback_tail + 1) % SCROLLBACK_LINES;
  }
}

static void vga_terminal_save_live_screen(void) {
  for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++) {
    live_screen[i] = terminal_buffer[i];
  }
}

static void vga_terminal_restore_live_screen(void) {
  for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++) {
    terminal_buffer[i] = live_screen[i];
  }
}

static void vga_terminal_update_display(void) {
  if (!scroll_mode || scroll_offset == 0) {
    vga_terminal_restore_live_screen();
    return;
  }

  if (scroll_offset > scrollback_count) {
    scroll_offset = scrollback_count;
  }
  if (scroll_offset > VGA_HEIGHT) {
    scroll_offset = VGA_HEIGHT;
  }

  for (size_t screen_y = 0; screen_y < VGA_HEIGHT; screen_y++) {
    if (screen_y < (size_t)scroll_offset) {
      int sb_idx = (scrollback_tail + (int)screen_y) % SCROLLBACK_LINES;
      
      for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[screen_y * VGA_WIDTH + x] = scrollback[sb_idx * VGA_WIDTH + x];
      }
    } else {
      size_t live_y = screen_y - scroll_offset;
      for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[screen_y * VGA_WIDTH + x] = live_screen[live_y * VGA_WIDTH + x];
      }
    }
  }
}

void vga_terminal_scroll(void) {
  vga_terminal_save_line_at_row(0);

  for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
      live_screen[y * VGA_WIDTH + x] = live_screen[(y + 1) * VGA_WIDTH + x];
    }
  }

  size_t last_row = (VGA_HEIGHT - 1) * VGA_WIDTH;
  for (size_t x = 0; x < VGA_WIDTH; x++) {
    terminal_buffer[last_row + x] = vga_entry(' ', terminal_color);
    live_screen[last_row + x] = vga_entry(' ', terminal_color);
  }
}

void vga_terminal_scroll_up(int lines) {
  if (scrollback_count == 0)
    return;

  if (!scroll_mode) {
    scroll_mode = true;
    vga_terminal_save_live_screen();
  }

  scroll_offset += lines;

  if (scroll_offset > scrollback_count)
    scroll_offset = scrollback_count;

  vga_terminal_update_display();
}

void vga_terminal_scroll_down(int lines) {
  if (!scroll_mode)
    return;

  scroll_offset -= lines;
  if (scroll_offset <= 0) {
    scroll_offset = 0;
    scroll_mode = false;
    vga_terminal_restore_live_screen();
    return;
  }

  vga_terminal_update_display();
}

void vga_terminal_scroll_bottom(void) {
  if (!scroll_mode)
    return;

  scroll_mode = false;
  scroll_offset = 0;
  vga_terminal_restore_live_screen();
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

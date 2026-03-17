#include <stdbool.h>

#include "arch/i386/cpu/ports.h"
#include "drivers/video/vga_text.h"
#include "libc/stdint.h"
#include "libc/string.h"

#define SCROLLBACK_LINES 1000

static size_t text_row;
static size_t text_column;
static uint8_t text_color;
static uint16_t* text_buffer = (uint16_t*)VGA_TEXT_MEMORY;

static uint16_t scrollback[SCROLLBACK_LINES * VGA_TEXT_WIDTH];
static int scrollback_head = 0;
static int scrollback_tail = 0;
static int scrollback_count = 0;

static bool scroll_mode = false;
static int scroll_offset = 0;

static uint16_t live_screen[VGA_TEXT_HEIGHT * VGA_TEXT_WIDTH];

static void vga_text_save_line_at_row(size_t row);
static void vga_text_save_live_screen(void);
static void vga_text_restore_live_screen(void);
static void vga_text_update_display(void);

static uint16_t vga_entry(char uc, uint8_t color) {
  return (uint16_t)uc | (uint16_t)color << 8;
}

void vga_text_initialise(void) {
  vga_text_clear_screen();
  scrollback_head = 0;
  scrollback_tail = 0;
  scrollback_count = 0;
  scroll_mode = false;
  scroll_offset = 0;
}

void vga_text_clear_screen(void) {
  text_row = 0;
  text_column = 0;
  text_color = vga_text_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

  for (size_t y = 0; y < VGA_TEXT_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_TEXT_WIDTH; x++) {
      const size_t index = y * VGA_TEXT_WIDTH + x;
      text_buffer[index] = vga_entry(' ', text_color);
      live_screen[index] = vga_entry(' ', text_color);
    }
  }
}

void vga_text_setcolor(uint8_t color) {
  text_color = color;
}

void vga_text_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_TEXT_WIDTH + x;
  text_buffer[index] = vga_entry(c, color);
}

#define HTAB_WIDTH 4
#define VTAB_WIDTH 4

void vga_text_putchar(char c) {
  if (scroll_mode) {
    scroll_mode = false;
    scroll_offset = 0;
    vga_text_restore_live_screen();
  }

  switch (c) {
    case '\n': {
      vga_text_save_line_at_row(text_row);

      const size_t index = text_row * VGA_TEXT_WIDTH + text_column;
      text_buffer[index] = vga_entry(' ', text_color);
      live_screen[index] = vga_entry(' ', text_color);

      text_column = 0;
      if (text_row < VGA_TEXT_HEIGHT - 1) {
        text_row++;
      } else {
        for (size_t y = 0; y < VGA_TEXT_HEIGHT - 1; y++) {
          for (size_t x = 0; x < VGA_TEXT_WIDTH; x++) {
            text_buffer[y * VGA_TEXT_WIDTH + x] = text_buffer[(y + 1) * VGA_TEXT_WIDTH + x];
            live_screen[y * VGA_TEXT_WIDTH + x] = live_screen[(y + 1) * VGA_TEXT_WIDTH + x];
          }
        }
        size_t last_row = (VGA_TEXT_HEIGHT - 1) * VGA_TEXT_WIDTH;
        for (size_t x = 0; x < VGA_TEXT_WIDTH; x++) {
          text_buffer[last_row + x] = vga_entry(' ', text_color);
          live_screen[last_row + x] = vga_entry(' ', text_color);
        }
      }
      return;
    }
    case '\t': {
      size_t offset = HTAB_WIDTH - (text_column % HTAB_WIDTH);
      text_column += offset ? offset : HTAB_WIDTH;
      return;
    }
    case '\r': {
      text_column = 0;
      return;
    }
    case '\f': {
      vga_text_clear_screen();
      return;
    }
    case '\b': {
      if (text_column != 0)
        text_column--;
      return;
    }
    case '\v': {
      size_t offset = VTAB_WIDTH - (text_row % VTAB_WIDTH);
      text_row += offset ? offset : VTAB_WIDTH;
      return;
    }
    case '\a': {
      return;
    }
    default:
      break;
  }

  const size_t index = text_row * VGA_TEXT_WIDTH + text_column;
  text_buffer[index] = vga_entry(c, text_color);
  live_screen[index] = vga_entry(c, text_color);

  text_column++;
  if (text_column >= VGA_TEXT_WIDTH) {
    text_column = 0;
    if (text_row < VGA_TEXT_HEIGHT - 1) {
      text_row++;
    } else {
      vga_text_save_line_at_row(0);
      for (size_t y = 0; y < VGA_TEXT_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_TEXT_WIDTH; x++) {
          text_buffer[y * VGA_TEXT_WIDTH + x] = text_buffer[(y + 1) * VGA_TEXT_WIDTH + x];
          live_screen[y * VGA_TEXT_WIDTH + x] = live_screen[(y + 1) * VGA_TEXT_WIDTH + x];
        }
      }
      size_t last_row = (VGA_TEXT_HEIGHT - 1) * VGA_TEXT_WIDTH;
      for (size_t x = 0; x < VGA_TEXT_WIDTH; x++) {
        text_buffer[last_row + x] = vga_entry(' ', text_color);
        live_screen[last_row + x] = vga_entry(' ', text_color);
      }
    }
  }
}

void vga_text_write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++)
    vga_text_putchar(data[i]);
}

void vga_text_writestring(const char* data) {
  vga_text_write(data, strlen(data));
}

static void vga_text_save_line_at_row(size_t row) {
  int dest_idx = scrollback_head * VGA_TEXT_WIDTH;

  for (size_t x = 0; x < VGA_TEXT_WIDTH; x++) {
    scrollback[dest_idx + x] = live_screen[row * VGA_TEXT_WIDTH + x];
  }

  scrollback_head = (scrollback_head + 1) % SCROLLBACK_LINES;

  if (scrollback_count < SCROLLBACK_LINES) {
    scrollback_count++;
  } else {
    scrollback_tail = (scrollback_tail + 1) % SCROLLBACK_LINES;
  }
}

static void vga_text_save_live_screen(void) {
  for (size_t i = 0; i < VGA_TEXT_HEIGHT * VGA_TEXT_WIDTH; i++) {
    live_screen[i] = text_buffer[i];
  }
}

static void vga_text_restore_live_screen(void) {
  for (size_t i = 0; i < VGA_TEXT_HEIGHT * VGA_TEXT_WIDTH; i++) {
    text_buffer[i] = live_screen[i];
  }
}

static void vga_text_update_display(void) {
  if (!scroll_mode || scroll_offset == 0) {
    vga_text_restore_live_screen();
    return;
  }

  if (scroll_offset > scrollback_count) {
    scroll_offset = scrollback_count;
  }
  if (scroll_offset > VGA_TEXT_HEIGHT) {
    scroll_offset = VGA_TEXT_HEIGHT;
  }

  for (size_t screen_y = 0; screen_y < VGA_TEXT_HEIGHT; screen_y++) {
    if (screen_y < (size_t)scroll_offset) {
      int sb_idx = (scrollback_tail + (int)screen_y) % SCROLLBACK_LINES;

      for (size_t x = 0; x < VGA_TEXT_WIDTH; x++) {
        text_buffer[screen_y * VGA_TEXT_WIDTH + x] = scrollback[sb_idx * VGA_TEXT_WIDTH + x];
      }
    } else {
      size_t live_y = screen_y - scroll_offset;
      for (size_t x = 0; x < VGA_TEXT_WIDTH; x++) {
        text_buffer[screen_y * VGA_TEXT_WIDTH + x] = live_screen[live_y * VGA_TEXT_WIDTH + x];
      }
    }
  }
}

void vga_text_scroll(void) {
  vga_text_save_line_at_row(0);

  for (size_t y = 0; y < VGA_TEXT_HEIGHT - 1; y++) {
    for (size_t x = 0; x < VGA_TEXT_WIDTH; x++) {
      text_buffer[y * VGA_TEXT_WIDTH + x] = text_buffer[(y + 1) * VGA_TEXT_WIDTH + x];
      live_screen[y * VGA_TEXT_WIDTH + x] = live_screen[(y + 1) * VGA_TEXT_WIDTH + x];
    }
  }

  size_t last_row = (VGA_TEXT_HEIGHT - 1) * VGA_TEXT_WIDTH;
  for (size_t x = 0; x < VGA_TEXT_WIDTH; x++) {
    text_buffer[last_row + x] = vga_entry(' ', text_color);
    live_screen[last_row + x] = vga_entry(' ', text_color);
  }
}

void vga_text_scroll_up(int lines) {
  if (scrollback_count == 0)
    return;

  if (!scroll_mode) {
    scroll_mode = true;
    vga_text_save_live_screen();
  }

  scroll_offset += lines;

  if (scroll_offset > scrollback_count)
    scroll_offset = scrollback_count;

  vga_text_update_display();
}

void vga_text_scroll_down(int lines) {
  if (!scroll_mode)
    return;

  scroll_offset -= lines;
  if (scroll_offset <= 0) {
    scroll_offset = 0;
    scroll_mode = false;
    vga_text_restore_live_screen();
    return;
  }

  vga_text_update_display();
}

void vga_text_scroll_bottom(void) {
  if (!scroll_mode)
    return;

  scroll_mode = false;
  scroll_offset = 0;
  vga_text_restore_live_screen();
}

uint16_t vga_text_get_entry_at(size_t x, size_t y) {
  return text_buffer[y * VGA_TEXT_WIDTH + x];
}

uint8_t vga_text_get_color(void) {
  return text_color;
}

void vga_text_get_cursor_position(size_t* x, size_t* y) {
  *x = text_column;
  *y = text_row;
}

#define VGA_CRTC_ADDRESS_PORT_COLOR 0x3D4
#define VGA_CRTC_DATA_PORT_COLOR 0x3D5
#define VGA_CRTC_CURSOR_START_INDEX 0x0A
#define VGA_CRTC_CURSOR_DISABLE_MASK 0x20

void vga_text_disable_cursor() {
  port_byte_out(VGA_CRTC_ADDRESS_PORT_COLOR, VGA_CRTC_CURSOR_START_INDEX);
  port_byte_out(VGA_CRTC_DATA_PORT_COLOR, VGA_CRTC_CURSOR_DISABLE_MASK);
}

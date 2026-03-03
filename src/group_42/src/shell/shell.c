#include "shell/shell.h"

#include <arch/i386/cpu/ports.h>
#include <kernel/input.h>
#include <stdio.h>

#include "drivers/input/keyboard.h"
#include "drivers/video/vga_terminal.h"
#include "kernel/util.h"
#include "shell/shell_command.h"

#define SHELL_BUFFER_SIZE 256
#define HISTORY_SIZE 10
#define PROMPT_STR "> "

static char cmd_buffer[SHELL_BUFFER_SIZE];
static size_t buf_cursor = 0;
static size_t buf_len = 0;

static char history[HISTORY_SIZE][SHELL_BUFFER_SIZE];
static size_t history_count = 0;
static int history_idx = -1;

static size_t prompt_len = 2;

static size_t cursor_x = 0;
static size_t cursor_y = 0;

static uint8_t terminal_color = 0;

static void buffer_clear(void) {
  for (size_t i = 0; i < SHELL_BUFFER_SIZE; i++) {
    cmd_buffer[i] = '\0';
  }
  buf_cursor = 0;
  buf_len = 0;
  history_idx = -1;
}

static void buffer_insert(char c) {
  if (buf_len >= SHELL_BUFFER_SIZE - 1) {
    return;
  }
  for (size_t i = buf_len; i > buf_cursor; i--) {
    cmd_buffer[i] = cmd_buffer[i - 1];
  }
  cmd_buffer[buf_cursor] = c;
  buf_cursor++;
  buf_len++;
  cmd_buffer[buf_len] = '\0';
}

static void buffer_delete(void) {
  if (buf_cursor == 0 || buf_len == 0) {
    return;
  }
  for (size_t i = buf_cursor - 1; i < buf_len - 1; i++) {
    cmd_buffer[i] = cmd_buffer[i + 1];
  }
  buf_cursor--;
  buf_len--;
  cmd_buffer[buf_len] = '\0';
}

static void buffer_home(void) {
  buf_cursor = 0;
}

static void buffer_end(void) {
  buf_cursor = buf_len;
}

static void buffer_move_left(void) {
  if (buf_cursor > 0) {
    buf_cursor--;
  }
}

static void buffer_move_right(void) {
  if (buf_cursor < buf_len) {
    buf_cursor++;
  }
}

static void history_add(const char* cmd) {
  if (buf_len == 0) {
    return;
  }
  for (size_t i = HISTORY_SIZE - 1; i > 0; i--) {
    for (size_t j = 0; j < SHELL_BUFFER_SIZE; j++) {
      history[i][j] = history[i - 1][j];
    }
  }
  for (size_t j = 0; j < SHELL_BUFFER_SIZE; j++) {
    history[0][j] = cmd_buffer[j];
  }
  if (history_count < HISTORY_SIZE) {
    history_count++;
  }
}

static void history_up(void) {
  if (history_count == 0) {
    return;
  }
  if (history_idx == -1) {
    history_idx = 0;
  } else if ((size_t)history_idx < history_count - 1) {
    history_idx++;
  }
  for (size_t j = 0; j < SHELL_BUFFER_SIZE; j++) {
    cmd_buffer[j] = history[history_idx][j];
    if (cmd_buffer[j] == '\0' && j < SHELL_BUFFER_SIZE) {
      for (size_t k = j; k < SHELL_BUFFER_SIZE; k++) {
        cmd_buffer[k] = '\0';
      }
      break;
    }
  }
  buf_len = 0;
  while (cmd_buffer[buf_len] != '\0' && buf_len < SHELL_BUFFER_SIZE - 1) {
    buf_len++;
  }
  buf_cursor = buf_len;
}

static void history_down(void) {
  if (history_idx == -1) {
    return;
  }
  if (history_idx == 0) {
    history_idx = -1;
    buffer_clear();
  } else {
    history_idx--;
    for (size_t j = 0; j < SHELL_BUFFER_SIZE; j++) {
      cmd_buffer[j] = history[history_idx][j];
    }
    buf_len = 0;
    while (cmd_buffer[buf_len] != '\0' && buf_len < SHELL_BUFFER_SIZE - 1) {
      buf_len++;
    }
    buf_cursor = buf_len;
  }
}

static void clear_line(size_t start_y) {
  for (size_t x = 0; x < 80; x++) {
    vga_terminal_putentryat(' ', terminal_color, x, start_y);
  }
}

static void render_line(void) {
  vga_get_cursor_position(&cursor_x, &cursor_y);
  size_t line_start_y = cursor_y;

  clear_line(line_start_y);

  for (size_t i = 0; PROMPT_STR[i] != '\0'; i++) {
    vga_terminal_putentryat(PROMPT_STR[i], terminal_color, i, line_start_y);
  }

  for (size_t i = 0; i < buf_len; i++) {
    vga_terminal_putentryat(cmd_buffer[i], terminal_color, prompt_len + i, line_start_y);
  }

  size_t new_cursor_x = prompt_len + buf_cursor;
  cursor_x = new_cursor_x;
  cursor_y = line_start_y;
}

static uint8_t cursor_saved_color = 0;
static void draw_cursor(void) {
  uint16_t entry = vga_terminal_get_entry_at(cursor_x, cursor_y);
  uint8_t orig_color = (entry >> 8) & 0xFF;

  cursor_saved_color = orig_color;

  uint8_t inverted = ((orig_color & 0xF0) >> 4) | ((orig_color & 0x0F) << 4);

  vga_terminal_putentryat((char)(entry & 0xFF), inverted, cursor_x, cursor_y);
}

static void clear_cursor(void) {
  uint16_t entry = vga_terminal_get_entry_at(cursor_x, cursor_y);

  vga_terminal_putentryat((char)(entry & 0xFF), cursor_saved_color, cursor_x, cursor_y);
}

// public:
void shell_init(void) {
  buffer_clear();
  vga_disable_cursor();
}


void shell_run(void) {
  shell_init();
  printf("Type help, to show commands.\n");

  size_t x, y;
  vga_get_cursor_position(&x, &y);
  cursor_x = 0;
  cursor_y = y;
  terminal_color = vga_terminal_get_color();
  render_line();
  draw_cursor();
  fflush(stdout);

  while (true) {
    decode_keyboard();

    while (has_special_key()) {
      uint8_t key = 0;
      pop_special_key(&key);

      clear_cursor();

      switch (key) {
        case KEY_LEFT: {
          buffer_move_left();
          break;
        }
        case KEY_RIGHT: {
          buffer_move_right();
          break;
        }
        case KEY_UP: {
          history_up();
          break;
        }
        case KEY_DOWN: {
          history_down();
          break;
        }
        case KEY_HOME: {
          buffer_home();
          break;
        }
        case KEY_END: {
          buffer_end();
          break;
        }
        case KEY_DELETE: {
          break;
        }
        default:
          break;
      }

      render_line();
      draw_cursor();
      fflush(stdout);
    }

    uint8_t c = 0;
    pop_key(&c);
    if (c != 0) {
      clear_cursor();

      if (c == '\n') {
        putchar('\n');
        if (buf_len > 0) {
          history_add(cmd_buffer);
          execute_command(cmd_buffer);
        }

        buffer_clear();

      } else if (c == '\b') {
        buffer_delete();
      } else if (c >= 32 && c <= 126) {
        buffer_insert(c);
      }

      render_line();
      draw_cursor();
      fflush(stdout);
    }
  }
}

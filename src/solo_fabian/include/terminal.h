#pragma once

/* The terminal writes text directly into the VGA text-mode framebuffer. */
void terminal_clear(void);

/* Write one character at the current cursor position. */
void terminal_putchar(char character);

/* Write a null-terminated string at the current cursor position. */
void terminal_write(const char* text);

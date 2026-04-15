#pragma once

#include "stdint.h"

#include "../vga_text_mode_interface/vga_text_mode_interface.h"

#define SHELL_MAX_USERNAME_LENGTH 24
#define SHELL_MAX_INPUT_LENGTH (VGA_TERMINAL_WIDTH - 4)

void shell_init(const char* username);
void shell_set_user(const char* username);
void shell_process_input(void);

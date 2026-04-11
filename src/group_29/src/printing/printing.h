#pragma once

#include "../vga_text_mode_interface/vga_text_mode_interface.h"



#define TEMP VGA_TERMINAL_WIDTH * sizeof(uint16_t) * 2

extern struct VgaTextModeInterface main_interface;

void init_vga_interface_for_printing();
void scroll_screen();

/// @brief Prints a char array (string) to "screen" by writing it to VGA buffer.
///
/// @param string The char array (string) to print. To include formatted values, use format_string as declared in format/format.h
/// @param color_bitmap The color the string should be displayed as. For this parameter use VgaColor(bg, fg) where bg/fg are VGA color enums.
void print(const char* string, uint8_t color_bitmap);
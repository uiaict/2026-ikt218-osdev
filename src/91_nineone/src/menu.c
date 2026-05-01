#include "menu.h"
#include "terminal.h"
#include "libc/string.h"
#include "colors.h"

void draw_window(const char* title) {
    uint8_t attr = COLOR(YELLOW, BLUE);

    // Corners
    terminal_putchar(201, attr, 0, 0);                       // ╔
    terminal_putchar(187, attr, VGA_WIDTH - 1, 0);            // ╗
    terminal_putchar(200, attr, 0, VGA_HEIGHT - 1);            // ╚
    terminal_putchar(188, attr, VGA_WIDTH - 1, VGA_HEIGHT - 1); // ╝

    // Horisontal borders
    for (int x = 1; x < VGA_WIDTH - 1; x++) {
        terminal_putchar(205, attr, x, 0);              // ═
        terminal_putchar(205, attr, x, VGA_HEIGHT - 1); // ═
    }

    // Vertical borders
    for (int y = 1; y < VGA_HEIGHT - 1; y++) {
        terminal_putchar(186, attr, 0, y);              // ║
        terminal_putchar(186, attr, VGA_WIDTH - 1, y);  // ║
    }

    // Title
    terminal_write("\xB9", attr, 2, 0); // ╠
    terminal_write(title, COLOR(YELLOW, BLUE), 3, 0);
    terminal_write("\xCC", attr, 3 + strlen(title), 0); // ╣
}

void draw() {
    draw_window("Main Menu");
}
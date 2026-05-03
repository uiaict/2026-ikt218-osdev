#include "menu.h"
#include "main_menu.h"
#include "apps/paint/paint.h"
#include "terminal.h"
#include "libc/string.h"
#include "libc/stdbool.h"
#include "arch/i386/isr.h"
#include "colors.h"
#include "keyboard.h"

int current_menu = MAIN_MENU; // 0 = main menu, 1 = paint program

void print_button(struct button* btn, bool is_selected, int x, int y) {
    int len = strlen(btn->label);

    uint8_t color_txt = is_selected ? COLOR(LIGHT_CYAN, BLACK) : COLOR(LIGHT_GREY, BLACK);
    uint8_t color_border = is_selected ? COLOR(WHITE, BLACK) : COLOR(LIGHT_GREY, BLACK);

    //  Top   part: ┌───────┐
    terminal_putchar(218, color_border, x, y); 
    for (int i = 0; i < len; i++) terminal_putchar(196, color_border, x + 1 + i, y);
    terminal_putchar(191, color_border, x + 1 + len, y);

    // Middle part: │ Label │
    terminal_putchar(179, color_border, x, y + 1);
    terminal_write(btn->label, color_txt, x + 1, y + 1);
    terminal_putchar(179, color_border, x + 1 + len, y + 1);

    // Bottom part: └───────┘
    terminal_putchar(192, color_border, x, y + 2);
    for (int i = 0; i < len; i++) terminal_putchar(196, color_border, x + 1 + i, y + 2);
    terminal_putchar(217, color_border, x + 1 + len, y + 2);
}

void keyboard_handler(registers_t* regs) {

    uint8 scancode = inb(0x60);

    // Vi bruker scancodes direkte for å være helt sikre (W=0x11, S=0x1F, Enter=0x1C)
    switch(current_menu) {
        case MAIN_MENU:
            handle_main_menu_keyboard(scancode);
            break;
        case PAINT_MENU:
            handle_paint_keyboard(scancode);
            break;
    }
}

void init_menu() {
    enter_main_menu();
    // Register keyboard handler for IRQ1 (keyboard interrupt)
    register_interrupt_handler(33, keyboard_handler);
}

void draw_window(const char* title) {
    uint8 color = COLOR(YELLOW, BLUE);

    // Corners
    terminal_putchar(201, color, 0, 0);                       // ╔
    terminal_putchar(187, color, VGA_WIDTH - 1, 0);            // ╗
    terminal_putchar(200, color, 0, VGA_HEIGHT - 1);            // ╚
    terminal_putchar(188, color, VGA_WIDTH - 1, VGA_HEIGHT - 1); // ╝

    // Horisontal borders
    for (int x = 1; x < VGA_WIDTH - 1; x++) {
        terminal_putchar(205, color, x, 0);              // ═
        terminal_putchar(205, color, x, VGA_HEIGHT - 1); // ═
    }

    // Vertical borders
    for (int y = 1; y < VGA_HEIGHT - 1; y++) {
        terminal_putchar(186, color, 0, y);              // ║
        terminal_putchar(186, color, VGA_WIDTH - 1, y);  // ║
    }

    // Title
    terminal_write("\xB9", color, 2, 0); // ╠
    terminal_write(title, COLOR(YELLOW, BLUE), 3, 0);
    terminal_write("\xCC", color, 3 + strlen(title), 0); // ╣
}

void draw_vertical_line(int x) {
    uint8 color = COLOR(YELLOW, BLUE);

    terminal_putchar(209, color, x, 0); // ╤

    for (int y = 1; y < VGA_HEIGHT - 1; y++) {
        terminal_putchar(179, color, x, y); // │
    }

    terminal_putchar(207, color, x, VGA_HEIGHT - 1); // ╧
}
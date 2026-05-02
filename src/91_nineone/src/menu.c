#include "menu.h"
#include "terminal.h"
#include "libc/string.h"
#include "libc/stdbool.h"
#include "arch/i386/isr.h"
#include "colors.h"

static int selected_item = 0;
#define NUM_OPTIONS 3


struct button {
    const char* label;
    void (*action)();
};

void test_action() {
    terminal_write("clicked 1st button", COLOR(BLUE, WHITE), 45, 16);
}

void test_actio2() {
    terminal_write("clicked 2nd button", COLOR(BLUE, WHITE), 45, 16);
}

void test_actio3() {
    terminal_write("clicked 3rd button", COLOR(BLUE, WHITE), 45, 16);
}

struct button start_menu[] = {
    {"Print info", test_action},
    {"Paint program", test_actio2},
    {"Play game", test_actio3}
};

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
    if (scancode & 0x80) return;

    // Vi bruker scancodes direkte for å være helt sikre (W=0x11, S=0x1F, Enter=0x1C)
    switch(scancode) {
        case 0x11: // W
            selected_item = (selected_item - 1 + NUM_OPTIONS) % NUM_OPTIONS;
            break;
        case 0x1F: // S
            selected_item = (selected_item + 1) % NUM_OPTIONS;
            break;
        case 0x1C: // Enter
            start_menu[selected_item].action();
            break;
    }
    draw_buttons();
}

void menu_init() {
    draw();
    // Register keyboard handler for IRQ1 (keyboard interrupt)
    register_interrupt_handler(33, keyboard_handler);
}

void draw_window(const char* title) {
    uint8 attr = COLOR(YELLOW, BLUE);

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

void draw_buttons() {

    int num_buttons = sizeof(start_menu) / sizeof(start_menu[0]);
    int start_x = 30;
    int start_y = 10;

    for (int i = 0; i < num_buttons; i++) {
        bool is_selected = (i == selected_item);
        print_button(&start_menu[i], is_selected, start_x, start_y + i * 4);
    }
}

void draw() {
    draw_window("Main Menu");
    draw_buttons();
}
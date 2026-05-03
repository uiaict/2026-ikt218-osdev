#include "apps/paint/paint.h"
#include "menu.h"
#include "main_menu.h"
#include "terminal.h"
#include "libc/stdio.h"
#include "libc/string.h"
#include "libc/stdbool.h"
#include "arch/i386/isr.h"
#include "colors.h"

#define NUM_OPTIONS 3

#define MODE_MENU 0
#define MODE_PAINT 1

#define BRUSH_PAINT 0
#define BRUSH_ERASE 1

static bool keys[128] = {false};

static int selected_item = 0;
static int mode = MODE_MENU;
static int x, y = 10;
static int brush = BRUSH_PAINT;

void test_actio2() {
    terminal_write("clicked 2nd button", COLOR(BLUE, WHITE), 45, 16);
}

void enter_paint_mode() {
    mode = MODE_PAINT;
    draw_cross(x, y);
}

struct button paint_menu[] = {
    {"Paint", enter_paint_mode},
    {"Save", test_actio2},
    {"Exit", enter_main_menu}
};

static void draw_buttons() {

    int num_buttons = sizeof(paint_menu) / sizeof(paint_menu[0]);
    int start_x = 66;
    int start_y = 10;

    for (int i = 0; i < num_buttons; i++) {
        bool is_selected = (i == selected_item);
        print_button(&paint_menu[i], is_selected, start_x, start_y + i * 3);
    }
}

void handle_paint_menu_keyboard(uint8 scancode) {
    switch (scancode) {
        case 0x11: // W
            selected_item = (selected_item - 1 + NUM_OPTIONS) % NUM_OPTIONS;
            break;
        case 0x1F: // S
            selected_item = (selected_item + 1) % NUM_OPTIONS;
            break;
        case 0x1C: // Enter
            paint_menu[selected_item].action();
            return;
    }
    draw_buttons();
}

void attempt_setchar(char c, int x, int y) {
    if (x < 2 || x > 62 || y < 2 || y > 22) return; // Outside paint area
    terminal_setchar(c, x, y); // Full block
}

void attempt_setcharfg(char c, uint8 color_fg, int x, int y) {
    if (x < 2 || x > 62 || y < 2 || y > 22) return; // Outside paint area
    terminal_setcharfg(c, color_fg, x, y); // Full block
}

void draw_cross(int x, int y) {
    uint8 color = brush == BRUSH_PAINT ? WHITE : RED;
    attempt_setcharfg(0xB3, color, x, y+1); // │
    attempt_setcharfg(0xB3, color, x, y-1);
    attempt_setcharfg(0xC4, color, x-1, y); // ─
    attempt_setcharfg(0xC4, color, x+1, y);
    attempt_setcharfg(0xC5, color, x, y); // ┼
}

void undraw_cross(int x, int y) {
    attempt_setchar(' ', x, y+1); // │
    attempt_setchar(' ', x, y-1);
    attempt_setchar(' ', x-1, y); // ─
    attempt_setchar(' ', x+1, y);
    attempt_setchar(' ', x, y); // ┼
}

void move_cross(int new_x, int new_y) {
    undraw_cross(x, y);
    x = new_x;  
    y = new_y;
    if (x > 62) x = 62;
    if (x < 2) x = 2;
    if (y < 2) y = 2;
    if (y > 22) y = 22;  
    draw_cross(x, y);
}

void handle_paint_mouse_keyboard(uint8 scancode) {

    if (keys[0x11]) move_cross(x, y-1); // W er nede
    if (keys[0x1F]) move_cross(x, y+1); // S er nede
    if (keys[0x1E]) move_cross(x-1, y); // A er nede
    if (keys[0x20]) move_cross(x+1, y); // D er nede
    int color = (brush == BRUSH_PAINT) ? WHITE : BLACK;
    
    if (keys[0x1C]) {terminal_setbgcolor(color, x, y);} // C er nede (clear)

    switch (scancode) {
        case 0x10: // Q: Go back to menu
            mode = MODE_MENU;
            draw_buttons();
            return;
        case 0x12: // E: Toggle Erase
            brush = (brush == BRUSH_PAINT) ? BRUSH_ERASE : BRUSH_PAINT;
            draw_cross(x, y); // Redraw cross with new color
            terminal_write(brush == BRUSH_PAINT ? "Mode: PAINT " : "Mode: ERASE ", COLOR(YELLOW, BLACK), 66, 3);
            break;
    }
}

void handle_paint_keyboard(uint8 scancode) {
    if (scancode & 0x80) {
        keys[scancode & 0x7F] = false; // Tast sluppe
    } else {
        keys[scancode] = true; // Tast trykt
    }
    switch (mode){
    case MODE_MENU:
        handle_paint_menu_keyboard(scancode);
        break;
    case MODE_PAINT:
        handle_paint_mouse_keyboard(scancode);
        break;
    }
}

void enter_paint_program() {
    terminal_clear(COLOR(WHITE, BLACK));
    draw_window("Paint Program");
    draw_vertical_line(64);
    draw_buttons();
    terminal_write(brush == BRUSH_PAINT ? "Mode: PAINT " : "Mode: ERASE ", COLOR(YELLOW, BLACK), 66, 3);
    current_menu = PAINT_MENU;
}
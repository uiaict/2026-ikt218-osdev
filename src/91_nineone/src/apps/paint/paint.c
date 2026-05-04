#include "apps/paint/paint.h"
#include "menu.h"
#include "main_menu.h"
#include "terminal.h"
#include "keyboard.h"
#include "libc/stdio.h"
#include "libc/string.h"
#include "libc/stdbool.h"
#include "arch/i386/isr.h"
#include "colors.h"
#include "kernel/memory.h"

#define NUM_OPTIONS 4

#define MODE_MENU 0
#define MODE_PAINT 1
#define MODE_COLOR_PICKER 2

#define BRUSH_PAINT 0
#define BRUSH_ERASE 1

#define COLOR_PICKER_START_X 66
#define COLOR_PICKER_START_Y 6

// The position of the brush is stored as an integer that is POS_MUL times the actual position to allow for smoother movement with velocity
// (since velocity can be less than 1 cell per tick, which would not move the brush at all if we only stored the actual cell position)
#define POS_MUL 200
#define VEL_LIM 20

#define CANVAS_WIDTH 61
#define CANVAS_HEIGHT 21
#define CANVAS_START_X 2
#define CANVAS_START_Y 2

static bool keys[128] = {false}; // Index is scancode, value is whether its currently pressed
// This makes it possible to check multiple keys at once (for smoother movement of the brush when holding down multiple keys)
// and to check if a key is being held down in the tick function for the brush.

static int selected_item = 0;
static int mode = MODE_MENU;
static int x, y = 10;
static int v_x, v_y = 0;
static int brush = BRUSH_PAINT;
static uint8 current_color = WHITE;
static int color_x = WHITE % 4;
static int color_y = WHITE / 4;

static uint8* saved_painting = NULL; // Store the color of each cell in the paint area for saving/loading

void save_changes() {
    terminal_write("Saving", COLOR(YELLOW, BLACK), 72, 14);
    for (size_t i = 0; i < CANVAS_HEIGHT; i++){
        for (size_t j = 0; j < CANVAS_WIDTH; j++){
            int x_pos = CANVAS_START_X + j; // Calculate the top-left corner of the cell
            int y_pos = CANVAS_START_Y + i;
            uint8 color = terminal_getbgcolor(x_pos, y_pos); // Get the color of the cell (assuming the entire cell is the same color)
            saved_painting[i*CANVAS_WIDTH + j] = color; // Save the color in the array
        }
    }
    terminal_write("Saved ", COLOR(GREEN, BLACK), 72, 14);
}

void load_saved_painting() {
    if (saved_painting == NULL) return; // No saved painting to load
    for (size_t i = 0; i < CANVAS_HEIGHT; i++){
        for (size_t j = 0; j < CANVAS_WIDTH; j++){
            int x_pos = CANVAS_START_X + j; // Calculate the top-left corner of the cell
            int y_pos = CANVAS_START_Y + i;
            uint8 color = saved_painting[i*CANVAS_WIDTH + j]; // Get the saved color from the array
            terminal_setbgcolor(color, x_pos, y_pos); // Set the cell to the saved color
        }
    }
    terminal_write("Loaded", COLOR(GREEN, BLACK), 72, 17);
}

void attempt_setchar(char c, int x, int y) {
    if (x < CANVAS_START_X || x > CANVAS_START_X + CANVAS_WIDTH - 1 || y < CANVAS_START_Y || y > CANVAS_START_Y + CANVAS_HEIGHT - 1) return; // Outside paint area
    terminal_setchar(c, x, y); // Full block
}

void attempt_setcharfg(char c, uint8 color_fg, int x, int y) {
    if (x < CANVAS_START_X || x > CANVAS_START_X + CANVAS_WIDTH - 1 || y < CANVAS_START_Y || y > CANVAS_START_Y + CANVAS_HEIGHT - 1) return; // Outside paint area
    terminal_setcharfg(c, color_fg, x, y); // Full block
}

void draw_cross() {
    int x_pos = x / POS_MUL;
    int y_pos = y / POS_MUL;
    uint8 color = brush == BRUSH_PAINT ? WHITE : RED;
    attempt_setcharfg(0xB3, color, x_pos, y_pos+1); // │
    attempt_setcharfg(0xB3, color, x_pos, y_pos-1);
    attempt_setcharfg(0xC4, color, x_pos-1, y_pos); // ─
    attempt_setcharfg(0xC4, color, x_pos+1, y_pos);
    attempt_setcharfg(0xC5, color, x_pos, y_pos); // ┼
}

void enter_paint_mode() {
    mode = MODE_PAINT;
    draw_cross();

    if (saved_painting == NULL) {
        saved_painting = (uint8*)malloc(sizeof(uint8) * CANVAS_HEIGHT * CANVAS_WIDTH);
    }
}

struct button paint_menu[] = {
    {"Paint", enter_paint_mode},
    {"Save", save_changes},
    {"Load", load_saved_painting},
    {"Exit", enter_main_menu}
};

static void draw_buttons() {
    int start_x = 66;
    int start_y = 11;

    for (int i = 0; i < NUM_OPTIONS; i++) {
        bool is_selected = (i == selected_item);
        print_button(&paint_menu[i], is_selected, start_x, start_y + i * 3);
    }
}

void handle_paint_menu_keyboard(char c) {
    terminal_write("      ", COLOR(GREEN, BLACK), 72, 14); // Clear "Saved"/"Loaded" message when navigating menu
    terminal_write("      ", COLOR(GREEN, BLACK), 72, 17);
    switch (c) {
        case 'w':
            selected_item = (selected_item - 1 + NUM_OPTIONS) % NUM_OPTIONS;
            break;
        case 's':
            selected_item = (selected_item + 1) % NUM_OPTIONS;
            break;
        case '\n': // Enter
            paint_menu[selected_item].action();
            return;
    }
    draw_buttons();
}

void draw_x(int x, int y) { // Draws an X over the color in the color picker
    int8 color = (y < 1) ? WHITE : BLACK; // Make sure the X is visible on top row (where all colors are dark)
    terminal_setcharfg(0x3E, color, COLOR_PICKER_START_X + x*2, COLOR_PICKER_START_Y + y); // >
    terminal_setcharfg(0x3C, color, COLOR_PICKER_START_X + x*2 + 1, COLOR_PICKER_START_Y + y); // <
}

void undraw_x(int x, int y) { // Clears the X over the color in the color picker (since the user has moved it to a different color)
    terminal_setchar(' ', COLOR_PICKER_START_X + x*2, COLOR_PICKER_START_Y + y);
    terminal_setchar(' ', COLOR_PICKER_START_X + x*2 + 1, COLOR_PICKER_START_Y + y);
}

void move_color_picker_cross(int new_x, int new_y) {
    undraw_x(color_x, color_y);
    color_x = new_x;
    color_y = new_y;
    if (color_x < 0) color_x = 0;
    if (color_x > 3) color_x = 3;
    if (color_y < 0) color_y = 0;
    if (color_y > 3) color_y = 3;
    draw_x(color_x, color_y);
}

void hide_color_picker() {
    terminal_write("Color:           ", COLOR(YELLOW, BLACK), COLOR_PICKER_START_X, COLOR_PICKER_START_Y - 1);
    terminal_write("  ", COLOR(current_color, current_color), COLOR_PICKER_START_X + 6, COLOR_PICKER_START_Y - 1);
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            terminal_putchar(' ', COLOR(BLACK, BLACK), COLOR_PICKER_START_X + j*2, COLOR_PICKER_START_Y + i);
            terminal_putchar(' ', COLOR(BLACK, BLACK), COLOR_PICKER_START_X + j*2 + 1, COLOR_PICKER_START_Y + i);
        }
    }
    terminal_write("Enter: paint", COLOR(BROWN, BLACK), COLOR_PICKER_START_X, COLOR_PICKER_START_Y + 1);
    terminal_write("C: set color", COLOR(BROWN, BLACK), COLOR_PICKER_START_X, COLOR_PICKER_START_Y + 2);
    terminal_write("E: togl erase", COLOR(BROWN, BLACK), COLOR_PICKER_START_X, COLOR_PICKER_START_Y + 3);
    terminal_write("Esc: menu", COLOR(BROWN, BLACK), COLOR_PICKER_START_X, COLOR_PICKER_START_Y + 4);
}

void handle_color_picker_menu_keyboard(char c) {
    switch(c) {
        case 'w':
            move_color_picker_cross(color_x, color_y - 1);
            break;
        case 's':
            move_color_picker_cross(color_x, color_y + 1);
            break;
        case 'a':
            move_color_picker_cross(color_x - 1, color_y);
            break;
        case 'd':
            move_color_picker_cross(color_x + 1, color_y);
            break;
        case '\n': // Enter
            mode = MODE_PAINT;
            brush = BRUSH_PAINT;
            current_color = 4 * color_y + color_x;
            terminal_write(brush == BRUSH_PAINT ? "Mode: PAINT " : "Mode: ERASE ", COLOR(YELLOW, BLACK), 66, 3);
            hide_color_picker();
            break;
    }
}

void show_color_picker() {
    terminal_write("Select Color", COLOR(YELLOW, BLACK), COLOR_PICKER_START_X, COLOR_PICKER_START_Y - 1);
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            terminal_putchar(' ', COLOR(WHITE, 4*i + j), COLOR_PICKER_START_X + j*2, COLOR_PICKER_START_Y + i);
            terminal_putchar(' ', COLOR(WHITE, 4*i + j), COLOR_PICKER_START_X + j*2 + 1, COLOR_PICKER_START_Y + i);
        }
    }
    draw_x(color_x, color_y); // Startposisjon for X
}

void undraw_cross() {
    int x_pos = x / POS_MUL;
    int y_pos = y / POS_MUL;
    attempt_setchar(' ', x_pos, y_pos+1); // │
    attempt_setchar(' ', x_pos, y_pos-1);
    attempt_setchar(' ', x_pos-1, y_pos); // ─
    attempt_setchar(' ', x_pos+1, y_pos);
    attempt_setchar(' ', x_pos, y_pos); // ┼
}

void move_cross(float new_x, float new_y) {
    undraw_cross();
    x = new_x;  
    y = new_y;
    if (x > 62*POS_MUL) x = 62*POS_MUL;
    if (x < 2*POS_MUL) x = 2*POS_MUL;
    if (y < 2*POS_MUL) y = 2*POS_MUL;
    if (y > 22*POS_MUL) y = 22*POS_MUL;
    draw_cross();
}

void tick_brush() {
    if (mode != MODE_PAINT) return;
    if (keys[0x11]) v_y -= 2; // W er nede
    if (keys[0x1F]) v_y += 2; // S er nede
    if (keys[0x1E]) v_x -= 3; // A er nede
    if (keys[0x20]) v_x += 3; // D er nede
    if (v_x | v_y) {
        move_cross(x + v_x, y + v_y);
        if (v_x != 0) v_x = (v_x > 3*VEL_LIM ? 3*VEL_LIM : v_x < -3*VEL_LIM ? -3*VEL_LIM : v_x) - ((v_x > 0) - (v_x < 0));
        if (v_y != 0) v_y = (v_y > 2*VEL_LIM ? 2*VEL_LIM : v_y < -2*VEL_LIM ? -2*VEL_LIM : v_y) - ((v_y > 0) - (v_y < 0));
    }
    int color = (brush == BRUSH_PAINT) ? current_color : BLACK;
    
    if (keys[0x1C]) {terminal_setbgcolor(color, x/POS_MUL, y/POS_MUL);} // Enter: paint/erase current cell
    printf("X: %d Y: %d  ", x, y); // Debug info for current position of the brush
    printf("vX: %d vY: %d  ", v_x, v_y); // Debug info for current position of the brush
    resetRowNumber(); // Reset row number after printf to avoid messing with paint area
}

void handle_paint_mode_keyboard(char c) {
    switch (c) {
        case 27: // Esc: Go back to menu
            mode = MODE_MENU;
            draw_buttons();
            return;
        case 'e': // E: Toggle Erase
            brush = (brush == BRUSH_PAINT) ? BRUSH_ERASE : BRUSH_PAINT;
            draw_cross(); // Redraw cross with new color
            terminal_write(brush == BRUSH_PAINT ? "Mode: PAINT " : "Mode: ERASE ", COLOR(YELLOW, BLACK), 66, 3);
            break;
        case 'c': // C: Open color picker
            mode = MODE_COLOR_PICKER;
            show_color_picker();
            break;
    }
}

void handle_paint_keyboard(uint8 scancode) {
    if (scancode & 0x80) {
        keys[scancode & 0x7F] = false; // Tast sluppe
    } else {
        keys[scancode] = true; // Tast trykt
    }
    char c = keyboard_scancode_to_ascii(scancode);
    switch (mode){
        case MODE_MENU:
            handle_paint_menu_keyboard(c);
            break;
        case MODE_PAINT:
            handle_paint_mode_keyboard(c);
            break;
        case MODE_COLOR_PICKER:
            handle_color_picker_menu_keyboard(c);
            break;
    }
}

void enter_paint_program() {
    terminal_clear(COLOR(WHITE, BLACK));
    draw_window("Paint Program");
    draw_vertical_line(64);
    draw_buttons();
    terminal_write(brush == BRUSH_PAINT ? "Mode: PAINT " : "Mode: ERASE ", COLOR(YELLOW, BLACK), 66, 3);
    hide_color_picker();
    current_menu = PAINT_MENU;
}
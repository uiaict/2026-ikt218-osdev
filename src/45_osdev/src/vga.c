#include <../include/vga.h>

static vga_state_t vga_state = {
    .foreground = VGA_COLOR_LIGHT_GREEN,
    .background = VGA_COLOR_BLACK,
    .cursor_x = 0,
    .cursor_y = 0,
};

// sets background color to black and text to green
void vga_init(void) {
    vga_state.foreground = VGA_COLOR_LIGHT_GREEN;
    vga_state.background = VGA_COLOR_BLACK;
    vga_state.cursor_x = 0;
    vga_state.cursor_y = 0;
    vga_clear();
}

// sets color
void vga_set_color(vga_color_t foreground, vga_color_t background) {
    vga_state.foreground = foreground;
    vga_state.background = background;
}

static uint8_t vga_make_color(uint8_t foreground, uint8_t background) {
    return (background << 4) | foreground;
}

static uint16_t vga_make_vram_entry(unsigned char uc, uint8_t color) {
    return ((uint16_t)uc) | (((uint16_t)color) << 8);
}

// putchar function with line wrapping and scrolling
void vga_putchar(char c) {
    uint8_t color = vga_make_color(vga_state.foreground, vga_state.background);
    uint16_t index = vga_state.cursor_y * VGA_WIDTH + vga_state.cursor_x;

    if (c == '\n') {
        vga_state.cursor_x = 0;
        vga_state.cursor_y++;
    } else if (c == '\r') {
        vga_state.cursor_x = 0;
    } else if (c == '\t') {
        vga_state.cursor_x += 4;
    } else {
        uint16_t *vram = (uint16_t *)VGA_MEMORY;
        vram[index] = vga_make_vram_entry(c, color);
        vga_state.cursor_x++;
    }

    // for line wrapping
    if (vga_state.cursor_x >= VGA_WIDTH) {
        vga_state.cursor_x = 0;
        vga_state.cursor_y++;
    }

    // for scrolling
    if (vga_state.cursor_y >= VGA_HEIGHT) {
        vga_state.cursor_y = 0;
    }
}

void vga_puts(const char *str) {
    while (*str) {
        vga_putchar(*str++);
    }
}
// write a single character at current cursor (used from IRQ handler)
void vga_putc(char c) {
    uint8_t color = vga_make_color(vga_state.foreground, vga_state.background);
    uint16_t *vram = (uint16_t *)VGA_MEMORY;

    if (c == '\n') {
        vga_state.cursor_x = 0;
        vga_state.cursor_y++;
    } else if (c == '\r') {
        vga_state.cursor_x = 0;
    } else if (c == '\t') {
        vga_state.cursor_x += 4;
    } else {
        uint16_t index = vga_state.cursor_y * VGA_WIDTH + vga_state.cursor_x;
        vram[index] = vga_make_vram_entry((unsigned char)c, color);
        vga_state.cursor_x++;
    }

    if (vga_state.cursor_x >= VGA_WIDTH) {
        vga_state.cursor_x = 0;
        vga_state.cursor_y++;
    }
    if (vga_state.cursor_y >= VGA_HEIGHT) {
        vga_state.cursor_y = 0;
    }
}

// Backspace functionality - remove previous character, update cursor and VRAM
void vga_backspace(void) {
    uint8_t color = vga_make_color(vga_state.foreground, vga_state.background);
    uint16_t *vram = (uint16_t *)VGA_MEMORY;

    // if at start of screen, nothing to do
    if (vga_state.cursor_x == 0 && vga_state.cursor_y == 0) return;

    // move cursor left, wrap to previous line if needed
    if (vga_state.cursor_x > 0) {
        vga_state.cursor_x--;
    } else {
        // at column 0 but not top line -> move to last column of previous line
        vga_state.cursor_y--;
        vga_state.cursor_x = VGA_WIDTH - 1;
    }

    // clear the character at the new cursor position
    uint16_t index = vga_state.cursor_y * VGA_WIDTH + vga_state.cursor_x;
    vram[index] = vga_make_vram_entry(' ', color);
}

// Clear the screen
void vga_clear(void) {
    uint8_t color = vga_make_color(vga_state.foreground, vga_state.background);
    uint16_t *vram = (uint16_t *)VGA_MEMORY;

    for (uint16_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vram[i] = vga_make_vram_entry(' ', color);
    }

    vga_state.cursor_x = 0;
    vga_state.cursor_y = 0;
}

void vga_set_cursor(uint16_t x, uint16_t y) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        vga_state.cursor_x = x;
        vga_state.cursor_y = y;
    }
}


/* Use local freestanding integer and size types. */
#include <libc/stdint.h>

#include <terminal.h>

/* VGA text mode has 80 columns by 25 rows. */
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/* VGA text memory starts at physical address 0xB8000 in protected mode. */
#define VGA_MEMORY ((volatile uint16_t*)0xB8000)

/* White/light grey foreground on a black background. */
#define VGA_COLOR_LIGHT_GREY_ON_BLACK 0x0F

/* Current cursor row in the VGA text buffer. */
static size_t terminal_row;

/* Current cursor column in the VGA text buffer. */
static size_t terminal_column;

/* Combine an ASCII character and a VGA color byte into one 16-bit screen cell. */
static uint16_t terminal_entry(char character, uint8_t color)
{
    return (uint16_t)character | (uint16_t)(color << 8);
}

/* Fill the whole VGA text screen with spaces and reset the cursor. */
void terminal_clear(void)
{
    /* Walk every row. */
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        /* Walk every column in the current row. */
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            /* Convert the 2D position into the 1D VGA memory index. */
            VGA_MEMORY[y * VGA_WIDTH + x] = terminal_entry(' ', VGA_COLOR_LIGHT_GREY_ON_BLACK);
        }
    }

    /* The next character should be written at the top-left corner. */
    terminal_row = 0;
    terminal_column = 0;
}

/* Write one character at the current cursor position. */
void terminal_putchar(char character)
{
    /* A newline moves to the first column of the next row. */
    if (character == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else {
        /* Store the character and color directly into VGA text memory. */
        VGA_MEMORY[terminal_row * VGA_WIDTH + terminal_column] =
            terminal_entry(character, VGA_COLOR_LIGHT_GREY_ON_BLACK);

        /* Advance to the next column after writing a normal character. */
        terminal_column++;
    }

    /* Wrap to the next row when reaching the right edge of the screen. */
    if (terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
    }

    /* This simple terminal wraps back to the top instead of scrolling. */
    if (terminal_row == VGA_HEIGHT) {
        terminal_row = 0;
    }
}

/* Write a null-terminated string to the VGA terminal. */
void terminal_write(const char* text)
{
    /* Keep writing characters until the C string's null terminator. */
    for (size_t i = 0; text[i] != '\0'; i++) {
        terminal_putchar(text[i]);
    }
}

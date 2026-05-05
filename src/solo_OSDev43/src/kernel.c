typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long size_t;
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "memory.h"
#include "paging.h"
#include "pit.h"
#include "songplayer.h"


extern uint32_t end;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint16_t*)0xB8000)


static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color = 0x0F; // white on black

static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

void terminal_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_putchar(char c) {
    if (c == '\b') {
    if (terminal_column > 0) {
        terminal_column--;
        VGA_MEMORY[terminal_row * VGA_WIDTH + terminal_column] =
            vga_entry(' ', terminal_color);
    }
    return;
    }
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        return;
    }

    VGA_MEMORY[terminal_row * VGA_WIDTH + terminal_column] =
        vga_entry(c, terminal_color);

    terminal_column++;

    if (terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
    }

    if (terminal_row >= VGA_HEIGHT) {
        terminal_row = 0;
    }
}

void terminal_write(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        terminal_putchar(str[i]);
    }
}
void print_int(int number) {
    char buffer[16];
    int i = 0;

    if (number == 0) {
        terminal_putchar('0');
        return;
    }

    if (number < 0) {
        terminal_putchar('-');
        number = -number;
    }

    while (number > 0) {
        buffer[i++] = '0' + (number % 10);
        number /= 10;
    }

    while (i > 0) {
        terminal_putchar(buffer[--i]);
    }
}

void printf(const char* format, int value) {
    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] == 'd') {
            print_int(value);
            i++;
        } else {
            terminal_putchar(format[i]);
        }
    }
}

void terminal_write_at(const char* str, size_t row, size_t column) {
    size_t x = column;

    while (*str) {
        VGA_MEMORY[row * VGA_WIDTH + x] = vga_entry(*str, terminal_color);
        str++;
        x++;
    }
}

void clear_line(size_t row) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        VGA_MEMORY[row * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
}

void piano_status(const char* note) {
    clear_line(12);
    terminal_write_at("Now playing: ", 12, 0);
    terminal_write_at(note, 12, 13);
}

void draw_piano_ui(void) {
    
    terminal_write("=======Mini Piano=======\n");
   
    terminal_write("Press keys 1-8 to play notes\n");
    terminal_write("Press SPACE to stop the current note\n\n");

    terminal_write("   1    2    3    4    5    6    7    8\n");
    terminal_write("+----+----+----+----+----+----+----+----+\n");
    terminal_write("| C4 | D4 | E4 | F4 | G4 | A4 | B4 | C5 |\n");
    terminal_write("+----+----+----+----+----+----+----+----+\n\n");

    
}

void test_new(void);

void main(void) {
    terminal_clear();

    gdt_init();
    idt_init();
    isr_install();
    irq_install();

    init_kernel_memory(&end);
    init_paging();
    print_memory_layout();
    init_pit();

    // terminal_write("Starting piano mode, press a number between 1-8 to play and press space to stop the note.\n"); // Old message, replaced by draw_piano_ui
    draw_piano_ui();
    // play_music(); // Uncomment to play music automatically

    while (1) {
        asm volatile("hlt");
    }
}
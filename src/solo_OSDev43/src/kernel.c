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

    void* a = malloc(12345);
    void* b = malloc(54321);
    void* c = malloc(13331);

    if (a && b && c) {
        terminal_write("malloc works\n");
    }

    free(b);

    void* d = malloc(1000);

    if (d == b) {
        terminal_write("free works\n");
    }

    test_new();

    terminal_write("Starting music player...\n");

    play_music();

    while (1) {
        asm volatile("hlt");
    }
}
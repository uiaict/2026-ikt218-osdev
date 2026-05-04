#include "stdint.h"
#include "libc/stdarg.h"
#include "isr.h"
#include "keyboard.h"
#include "memory.h"
#include "pit.h"
#include "song.h"
#include "ports.h"
#include "shell.h"

// This is defined in arch/i386/linker.ld
extern uint32_t end;

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gp;

extern void gdt_flush(uint32_t);

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_install() {
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (uint32_t)&gdt;
    
    gdt_set_gate(0, 0, 0, 0, 0);                // NULL Segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // CODE Segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // DATA Segment
    
    gdt_flush((uint32_t)&gp);
}


int cursor_x = 0;
int cursor_y = 0;

void putchar(char c) {
    volatile uint16_t* video = (volatile uint16_t*)0xB8000;
    
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') { // Backspace (Geri Silme) yetenegi
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = 79;
        }
        video[cursor_y * 80 + cursor_x] = (uint16_t)' ' | 0x0F00;
    } else {
        video[cursor_y * 80 + cursor_x] = (uint16_t)c | 0x0F00;
        cursor_x++;
    }

    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }

    // Scroll the screen if we reach the bottom (25th row)
    if (cursor_y >= 25) {
        for (int i = 0; i < 24 * 80; i++) {
            video[i] = video[i + 80];
        }
        for (int i = 24 * 80; i < 25 * 80; i++) {
            video[i] = (uint16_t)' ' | 0x0F00;
        }
        cursor_y = 24;
    }
}

void terminal_write(const char* str) {
    while(*str) putchar(*str++);
}

char* itoa(int value, char* str, int base) {
    char *rc, *ptr, *low;
    if (base < 2 || base > 36) return str;
    rc = ptr = str;
    if (value < 0 && base == 10) {
        *ptr++ = '-';
    }
    low = ptr;
    do {
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[value % base];
        value /= base;
    } while (value);
    *ptr-- = '\0';
    while (low < ptr) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    for (const char* p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            if (*p == 'd') {
                int i = va_arg(args, int);
                char s[32];
                itoa(i, s, 10);
                terminal_write(s);
            } else if (*p == 's') {
                char* s = va_arg(args, char*);
                terminal_write(s);
            } else if (*p == 'c') {
                char c = (char)va_arg(args, int);
                putchar(c);
            } else if (*p == 'x') {
                int i = va_arg(args, int);
                char s[32];
                itoa(i, s, 16);
                terminal_write(s);
            }
        } else {
            putchar(*p);
        }
    }

    va_end(args);
}

void main(void) {
    gdt_install();
    isr_install(); // Installs IDT, CPU Exceptions, and IRQs
    
    // Task 4: Initialize the keyboard listener
    init_keyboard();

    // Initialize the kernel's memory manager using the end address of the kernel.
    init_kernel_memory(&end);

    // Initialize paging for memory management.
    init_paging();

    // Initialize PIT
    init_pit();

    // CRITICAL: Enable external hardware interrupts.
    // 'sti' stands for Set Interrupt Flag. It tells the CPU to start listening.
    __asm__ volatile("sti");

    shell_init();

    while (1) {
        shell_update();
        __asm__ volatile("hlt");
    }
}

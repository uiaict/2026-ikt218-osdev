#include "../gdt.h"
#include "../idt.h"
#include "../irq.h"
#include "../include/io.h"
#include "../include/print.h"
#include <libc/stdint.h>

#define VGA_TEXT_BUFFER 0xb8000u
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_CTRL_PORT   0x3D4
#define VGA_DATA_PORT   0x3D5


static volatile uint16_t *vp = (uint16_t *)VGA_TEXT_BUFFER + 2;

void print_string(const char *s, uint8_t attrib)
{
	while (*s) {
		*vp = (attrib << 8) | *s++;
        vp++;
    }
    update_cursor();
}

void scroll() {
    volatile uint16_t *buf = (volatile uint16_t *)VGA_TEXT_BUFFER;
    for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
        buf[i] = buf[i + VGA_WIDTH];
    }
    for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++) {
        buf[i] = (0x07 << 8) | ' ';
    }
    vp = buf + VGA_WIDTH * (VGA_HEIGHT - 1);
}

void print_newline() {
    // Find how far into the current line we are
    uint32_t current = vp - (volatile uint16_t *)VGA_TEXT_BUFFER;
    // Skip forward to the next multiple of 80
    vp += VGA_WIDTH - (current % VGA_WIDTH);

    // If we've gone past the last row, scroll instead of wrapping
    if (vp >= (volatile uint16_t *)VGA_TEXT_BUFFER + (VGA_WIDTH * VGA_HEIGHT)) {
        scroll();
        update_cursor();
    }

}
void update_cursor() {
    uint32_t pos = vp - (volatile uint16_t *)VGA_TEXT_BUFFER;
    
    outb(VGA_CTRL_PORT, 0x0F);
    outb(VGA_DATA_PORT, (uint8_t)(pos & 0xFF));
    outb(VGA_CTRL_PORT, 0x0E);
    outb(VGA_DATA_PORT, (uint8_t)((pos >> 8) & 0xFF));
}

void print_backspace() {
    vp--;
    *vp = (0x07 << 8) | ' ';
    update_cursor();
}

void main() {
    gdt_init();
    idt_init();   
    irq_init(); 
    asm("sti");  // enable interrupts
    print_string("Hello World ", 0x07); 
    print_newline();

}
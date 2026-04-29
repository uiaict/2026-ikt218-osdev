#include "../gdt.h"
#include <libc/stdint.h>
#define VGA_TEXT_BUFFER 0xb8000u

void print_string(const char *s, uint8_t attrib)
{
	volatile uint16_t *vp = (uint16_t *)VGA_TEXT_BUFFER;
	while (*s) {
		*vp = (attrib << 8) | *s++;
        vp++;
    }
}

void main() {
    gdt_init();
    print_string("Hello World", 0x07);
}
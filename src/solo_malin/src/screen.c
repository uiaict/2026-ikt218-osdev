#include <libc/stdint.h>
#include "screen.h"

// VGS text mode buffer starts at physical address 0xB8000
// Each 16-bit entry: high byte = color attribute, low byte = ASCII character.
#define VGA ((unsigned short*)0xB8000)

//Simple function to write a null-terminated string to the top of the screen.
void write_string(const char *s) {
    static unsigned int pos = 0;
    unsigned int i = 0;
    while (s[i]) {
        VGA[pos++] = 0x0F00 | (unsigned short)s[i];
        i++;
    }
}
#include <gdt.h>
#include <libc/stdio.h>

void kernel_main(void) {
    gdt_init();
    printf("HELLO WORLD");

    for (;;) {
    }
}
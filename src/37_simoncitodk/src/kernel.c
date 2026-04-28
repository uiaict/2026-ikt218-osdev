#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "terminal.h"

void kernel_main(void)
{
    gdt_init();
    idt_init();
    pic_remap();
    pic_mask_all_except_keyboard();

    terminal_write("Interrupts ready\n");
    terminal_write("Press keys to trigger IRQ1\n");

    asm volatile("sti");

    for (;;) {
        asm volatile("hlt");
    }
}

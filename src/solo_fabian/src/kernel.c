/* GDT setup must happen before the kernel relies on its own segment layout. */
#include <gdt.h>

/* IDT setup allows the kernel to receive CPU interrupts and hardware IRQs. */
#include <idt.h>

/* Terminal output is shared by the kernel and interrupt handlers. */
#include <terminal.h>

#include <memory.h>

extern uint32_t end;

/* Kernel entry point called from multiboot2.asm. */
void main(void) {
    /* Install the kernel's GDT before doing regular kernel work. */
    gdt_init();

    /* Start from a blank VGA text screen. */
    terminal_clear();

    /* Required visible startup message. */
    terminal_write("Hello World\n");

    /* Install the IDT and configure IRQ0..IRQ15 through the PIC. */
    idt_init();

    init_kernel_memory(&end);
    init_paging();

    terminal_write("Before malloc:\n");
    print_memory_layout();

    void* memory1 = malloc(12345);
    void* memory2 = malloc(54321);
    void* memory3 = malloc(13331);

    print_pointer("memory1: ", memory1);
    print_pointer("memory2: ", memory2);
    print_pointer("memory3: ", memory3);

    terminal_write("After malloc:\n");
    print_memory_layout();

    free(memory2);

    void* memory4 = malloc(1000);
    print_pointer("memory4: ", memory4);

    /* Trigger three software interrupts to show that the ISR path works. */
    // __asm__ volatile ("int $0x0");
    // __asm__ volatile ("int $0x1");
    // __asm__ volatile ("int $0x2");

    /* Enable maskable hardware interrupts, including timer and keyboard IRQs. */
    // __asm__ volatile ("sti");

    /* Halt forever... Interrupts will wake the CPU when hardware events arrive. */
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

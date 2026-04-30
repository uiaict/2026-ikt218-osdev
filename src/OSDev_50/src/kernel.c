#include <libc/stdint.h>
#include <libc/stdio.h>

#include "gdt.h"
#include "terminal.h"
#include "descriptor_tables.h"
#include "interrupts.h"
#include "keyboard.h"
#include "memory.h"
#include "pit.h"

extern uint32_t end;

void main(uint32_t mb_magic, uint32_t mb_info_addr)
{
    (void)mb_magic;
    (void)mb_info_addr;

    
    terminal_initialize();
    terminal_write("Hello World\n");

    gdt_init();

    init_idt();
    init_irq();

    isr_register_default_prints();
    keyboard_init();

    init_kernel_memory(&end);
    init_paging();

    printf("Before malloc:\n");
    print_memory_layout();

    void* a = malloc(1000);
    void* b = malloc(2000);

    printf("After malloc:\n");
    print_memory_layout();

    free(a);
    free(b);

    printf("After free:\n");
    print_memory_layout();

    init_pit();

    __asm__ volatile ("sti");

    int counter = 0;

    for (int i = 0; i < 5; i++) {
        printf("[%d] busy sleep 1s\n", counter++);
        sleep_busy(1000);

        printf("[%d] interrupt sleep 1s\n", counter++);
        sleep_interrupt(1000); }

        printf("Done. Halting...\n");
            while (1) {
            __asm__ volatile ("hlt"); }
}

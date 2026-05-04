#include <libc/stdbool.h>
#include <libc/stdint.h>
#include <libc/stdio.h>
#include <idt.h>
#include <kernel/memory.h>
#include <kernel/pit.h>

extern uint32_t end;

void main(uint32_t magic, uint32_t mb_addr)
{
    (void)mb_addr;

    terminal_clear();
    terminal_set_color(VGA_COLOR_LCYAN);
    printf("===  UiAOS - 26_RTX  ===\n\n");
    terminal_set_color(VGA_COLOR_BWHITE);

    if (magic != 0x36d76289) {
        terminal_set_color(VGA_COLOR_LRED);
        printf("ERROR: Not loaded by a Multiboot2 bootloader (magic=0x%x)\n", magic);
        for (;;) __asm__ volatile ("hlt");
    }

    printf("[INIT] Setting up Interrupt Descriptor Table...\n");
    idt_init();
    printf("[OK]   IDT loaded (256 entries, ISRs 0-31, IRQs 0-15).\n\n");

    printf("[TEST] Triggering software interrupts:\n");

    terminal_set_color(VGA_COLOR_YELLOW);
    printf("  INT 0x01 (Debug):\n    ");
    terminal_set_color(VGA_COLOR_BWHITE);
    __asm__ volatile ("int $0x1");

    terminal_set_color(VGA_COLOR_YELLOW);
    printf("  INT 0x03 (Breakpoint):\n    ");
    terminal_set_color(VGA_COLOR_BWHITE);
    __asm__ volatile ("int $0x3");

    terminal_set_color(VGA_COLOR_YELLOW);
    printf("  INT 0x04 (Overflow):\n    ");
    terminal_set_color(VGA_COLOR_BWHITE);
    __asm__ volatile ("int $0x4");

    printf("\n[OK]   All test ISRs returned normally.\n\n");

    init_kernel_memory(&end);
    init_paging();
    print_memory_layout();

    printf("[INIT] Initializing PIT (1000 Hz)...\n");
    init_pit();
    printf("[OK]   PIT ready.\n\n");

    printf("[INIT] Enabling hardware interrupts (STI)...\n");
    __asm__ volatile ("sti");
    printf("[OK]   IRQ0 (timer) and IRQ1 (keyboard) are now active.\n\n");

    printf("Hello World!\n");
    void *some_memory = malloc(12345);
    void *memory2     = malloc(54321);
    void *memory3     = malloc(13331);
    (void)some_memory; (void)memory2; (void)memory3;

    int counter = 0;
    while (true) {
        printf("[%d]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
        sleep_busy(1000);
        printf("[%d]: Slept using busy-waiting.\n", counter++);

        printf("[%d]: Sleeping with interrupts (LOW CPU).\n", counter);
        sleep_interrupt(1000);
        printf("[%d]: Slept using interrupts.\n", counter++);
    }
}

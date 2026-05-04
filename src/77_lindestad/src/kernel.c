#include <gdt.h>
#include <interrupts.h>
#include <libc/stdint.h>
#include <libc/stdio.h>
#include <memory.h>
#include <pit.h>
#include <terminal.h>

extern uint32_t end;

int main(uint32_t multiboot_magic, uint32_t multiboot_info_addr)
{
    (void)multiboot_magic;
    (void)multiboot_info_addr;

    gdt_initialize();
    terminal_initialize();
    printf("Hello World\n");

    init_kernel_memory(&end);
    init_paging();
    print_memory_layout();

    void* some_memory = malloc(12345);
    void* memory2 = malloc(54321);
    void* memory3 = malloc(13331);
    printf("malloc blocks: 0x%x 0x%x 0x%x\n", (uint32_t)some_memory, (uint32_t)memory2, (uint32_t)memory3);
    test_new_operator();

    interrupts_initialize();

    __asm__ volatile("int $0x0");
    __asm__ volatile("int $0x1");
    __asm__ volatile("int $0x2");

    init_pit();
    __asm__ volatile("sti");

    uint32_t counter = 0;
    for (;;) {
        printf("[%u]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
        sleep_busy(1000);
        printf("[%u]: Slept using busy-waiting.\n", counter++);

        printf("[%u]: Sleeping with interrupts (LOW CPU).\n", counter);
        sleep_interrupt(1000);
        printf("[%u]: Slept using interrupts.\n", counter++);
    }
}

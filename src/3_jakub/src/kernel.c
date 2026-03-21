#include "libc/libs.h"
#include <multiboot2.h>
#include "gdt.h"
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "interrupts/keyboard.h"

struct multiboot_info
{
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

int kernel_main();

int compute(int a, int b)
{
    return a + b;
}

int main(uint32_t magic, struct multiboot_info *mb_info_addr)
{
    int result = compute(2, 3);
    // printf("Result of compute(2, 3) is: %d\n", result);

    init_gdt();
    
    // Initialize IDT
    init_idt();
    
    // Initialize Interrupts and IRQs
    init_isr();
    init_irq();

    // Enable CPU Interrupts
    asm volatile("sti");

    // Initialize the keyboard logger
    init_keyboard();

    printf("Hello world!\n");
    
    // Test interrupt
    printf("Triggering a test interrupt (INT 0x03)...\n");
    asm volatile("int $0x03");
    
    printf("Interrupt test complete. Try typing on the keyboard!\n");

    // Call cpp kernel_main (defined in kernel.cpp)
    return kernel_main();
}

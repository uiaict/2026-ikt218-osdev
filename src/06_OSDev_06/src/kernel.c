#include <libc/stdint.h>
#include <libc/stdio.h>
#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <keyboard.h>
#include <terminal.h>

/*
 * isr_demo_handler - shared demo handler for software-triggered interrupts
 *
 * Installed on ISR 0, 3, and 6 before they are triggered, so the kernel
 * can print a message and continue rather than halting.
 *
 * Note: using `int $0` (explicit software interrupt) is safe to return from
 * because the CPU pushes the address of the instruction AFTER int $0.
 * A real divide-by-zero (#DE) would push the faulting instruction address
 * and loop forever on iret — for demonstration purposes `int $0` is correct.
 */
static void isr_demo_handler(registers_t *regs)
{
    terminal_writecolor("  [ISR] Caught interrupt ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    printf("%d", regs->int_no);
    terminal_writecolor(" - returned safely.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
}

/*
 * main - kernel entry point
 *
 * Called from _start (src/multiboot2.asm) after the bootloader stack is set up.
 *
 * Initialisation order:
 *   1. GDT  — valid segment descriptors must exist before any fault can be handled.
 *   2. Terminal — needed for all diagnostic output that follows.
 *   3. IDT  — installs exception and IRQ gates; interrupts still disabled.
 *   4. IRQ  — remaps the 8259 PIC so hardware IRQs land at 0x20-0x2F.
 *   5. Keyboard — registers the IRQ1 handler.
 *   6. sti  — enable hardware interrupts.
 */
void main(uint32_t magic, uint32_t mbi)
{
    (void)magic;
    (void)mbi;

    /* --- Hardware initialisation --- */
    gdt_init();
    terminal_init();

    terminal_writecolor("=== UiA OS - Assignment 3 ===\n\n", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);

    printf("GDT loaded.\n");

    idt_init();
    printf("IDT loaded.\n");

    irq_init();
    printf("PIC remapped (IRQ 0-15 -> INT 0x20-0x2F).\n");

    keyboard_init();
    printf("Keyboard handler installed on IRQ1.\n\n");

    /* --- Install demo handlers BEFORE triggering software interrupts --- */
    isr_install_handler(0, isr_demo_handler);  /* Division Error       */
    isr_install_handler(3, isr_demo_handler);  /* Breakpoint           */
    isr_install_handler(6, isr_demo_handler);  /* Invalid Opcode       */

    /* --- Enable hardware interrupts --- */
    __asm__ volatile ("sti");
    printf("Interrupts enabled.\n\n");

    /* --- Task 2: demonstrate three ISRs via software interrupts --- */
    terminal_writecolor("--- ISR Demo ---\n", VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);

    printf("Triggering ISR 0  (Division Error)...\n");
    __asm__ volatile ("int $0");

    printf("Triggering ISR 3  (Breakpoint)...\n");
    __asm__ volatile ("int $3");

    printf("Triggering ISR 6  (Invalid Opcode)...\n");
    __asm__ volatile ("int $6");

    terminal_writecolor("\nAll ISRs handled successfully.\n\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);

    /* --- Ready for keyboard input --- */
    terminal_writecolor("--- Keyboard Logger Active ---\n", VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    printf("Type below (Backspace supported):\n> ");

    /*
     * Spin with hlt: the CPU sleeps until an interrupt wakes it.
     * Each keypress fires IRQ1, which calls keyboard_handler, which
     * reads the scancode, translates it to ASCII, and prints it.
     */
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

#include <isr.h>
#include <idt.h>
#include <libc/stdio.h>
#include <libc/stdbool.h>

static const char *exception_names[] = {
    "Division By Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point",
    "Virtualization",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved",
};

static bool is_fatal(uint32_t n) {
    return (n == 0 || n == 8 || n == 13 || n == 14);
}

void isr_handler(registers_t *regs) {
    terminal_set_color(VGA_COLOR_LRED);
    printf("[ISR %d] %s", regs->int_no, exception_names[regs->int_no]);
    if (regs->err_code)
        printf(" (error code: 0x%x)", regs->err_code);
    printf("\n");
    terminal_set_color(VGA_COLOR_BWHITE);

    if (is_fatal(regs->int_no)) {
        terminal_set_color(VGA_COLOR_LRED);
        printf("FATAL - system halted.\n");
        for (;;) __asm__ volatile ("hlt");
    }
}

void isr_init(uint16_t cs) {
    idt_set_gate( 0, (uint32_t)isr0,  cs, 0x8E);
    idt_set_gate( 1, (uint32_t)isr1,  cs, 0x8E);
    idt_set_gate( 2, (uint32_t)isr2,  cs, 0x8E);
    idt_set_gate( 3, (uint32_t)isr3,  cs, 0x8E);
    idt_set_gate( 4, (uint32_t)isr4,  cs, 0x8E);
    idt_set_gate( 5, (uint32_t)isr5,  cs, 0x8E);
    idt_set_gate( 6, (uint32_t)isr6,  cs, 0x8E);
    idt_set_gate( 7, (uint32_t)isr7,  cs, 0x8E);
    idt_set_gate( 8, (uint32_t)isr8,  cs, 0x8E);
    idt_set_gate( 9, (uint32_t)isr9,  cs, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, cs, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, cs, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, cs, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, cs, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, cs, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, cs, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, cs, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, cs, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, cs, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, cs, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, cs, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, cs, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, cs, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, cs, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, cs, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, cs, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, cs, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, cs, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, cs, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, cs, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, cs, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, cs, 0x8E);
}

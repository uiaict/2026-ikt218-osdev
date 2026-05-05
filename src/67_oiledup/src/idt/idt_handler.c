#include "kernel/idt.h"
#include "libc/stdio.h"
#include "libc/system.h"
#include "kernel/keyboard.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

void interrupt_handler(registers_t *regs)
{
    if (interrupt_handlers[regs->int_no] != 0)
    {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    }
    else
    {
        printf("Unhandled interrupt: %d, Error code: %d\n", regs->int_no, regs->err_code);
        printf("EIP: 0x%x, CS: 0x%x, EFLAGS: 0x%x\n", regs->eip, regs->cs, regs->eflags);
        if (regs->int_no < 32) {
            panic("CPU Exception");
        }
    }
}

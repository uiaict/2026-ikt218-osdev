#include <isr.h>
#include <libc/stdio.h>
#include <terminal.h>

/*
 * Table of registered C handlers for CPU exceptions 0-31.
 * Initialised to NULL (zero) by the C runtime (.bss section).
 * When NULL, isr_dispatch falls back to the default error message.
 */
static isr_handler_t isr_handlers[32];

/*
 * Human-readable names for the 32 CPU exception vectors.
 * Index = vector number.
 */
static const char *exception_messages[] = {
    "Division By Zero",           /*  0 #DE */
    "Debug",                      /*  1 #DB */
    "Non-Maskable Interrupt",     /*  2     */
    "Breakpoint",                 /*  3 #BP */
    "Overflow",                   /*  4 #OF */
    "Bound Range Exceeded",       /*  5 #BR */
    "Invalid Opcode",             /*  6 #UD */
    "Device Not Available",       /*  7 #NM */
    "Double Fault",               /*  8 #DF */
    "Coprocessor Segment Overrun",/*  9     */
    "Invalid TSS",                /* 10 #TS */
    "Segment Not Present",        /* 11 #NP */
    "Stack-Segment Fault",        /* 12 #SS */
    "General Protection Fault",   /* 13 #GP */
    "Page Fault",                 /* 14 #PF */
    "Reserved",                   /* 15     */
    "x87 Floating-Point Exception",/* 16 #MF */
    "Alignment Check",            /* 17 #AC */
    "Machine Check",              /* 18 #MC */
    "SIMD Floating-Point Exception",/* 19 #XM */
    "Virtualization Exception",   /* 20 #VE */
    "Control Protection Exception",/* 21 #CP */
    "Reserved",                   /* 22     */
    "Reserved",                   /* 23     */
    "Reserved",                   /* 24     */
    "Reserved",                   /* 25     */
    "Reserved",                   /* 26     */
    "Reserved",                   /* 27     */
    "Hypervisor Injection",        /* 28 #HV */
    "VMM Communication",          /* 29 #VC */
    "Security Exception",         /* 30 #SX */
    "Reserved",                   /* 31     */
};

void isr_install_handler(uint8_t isr_num, isr_handler_t handler)
{
    if (isr_num < 32) {
        isr_handlers[isr_num] = handler;
    }
}

/*
 * isr_dispatch - C-level exception dispatcher
 *
 * Called from isr_common_stub in interrupt_stubs.asm with a pointer to
 * the full CPU register state on the stack.
 *
 * If a custom handler is registered for this vector, call it and return.
 * Otherwise print a diagnostic message and halt — an unhandled CPU exception
 * is a fatal kernel error.
 */
void isr_dispatch(registers_t *regs)
{
    if (regs->int_no < 32 && isr_handlers[regs->int_no]) {
        isr_handlers[regs->int_no](regs);
        return;
    }

    /* Default: print exception info in red, then halt */
    terminal_writecolor("\n[EXCEPTION] ", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    if (regs->int_no < 32) {
        printf("%s", exception_messages[regs->int_no]);
    } else {
        printf("Unknown interrupt %d", regs->int_no);
    }
    printf("\n");
    printf("  int=%d  err=0x%x  eip=0x%x  cs=0x%x  eflags=0x%x\n",
           regs->int_no, regs->err_code, regs->eip, regs->cs, regs->eflags);
    printf("  eax=0x%x  ebx=0x%x  ecx=0x%x  edx=0x%x\n",
           regs->eax, regs->ebx, regs->ecx, regs->edx);

    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}

#pragma once
#include <libc/stdint.h>


// ISRs reserved for CPU exceptions
#define ISR_CPU_COUNT 32

// IRQ definitions
#define IRQ_COUNT 16

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47


/**
 * Struct representing the CPU registers. Useful for system calls, and interrupt service routines
 */
typedef struct {
  uint32_t ds;                                         /* Data segment selector */
  uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax; /* Pushed by pusha. */
  uint32_t int_no, err_code;         /* Interrupt number and error code (if applicable) */
  uint32_t eip, cs, eflags, esp, ss; /* Pushed by the processor automatically */
} registers_t;

/**
 * Initializes the interrupt service
 */
void init_isrs();

/**
 * Default interrupt service routine handler.
 * Just prints exception names
 * @param regs
 */
void isr_handler(registers_t* regs);

/**
 * Initializes and registers interrupt requests, such as the PIT, and the PS/2 Keyboard
 */
void init_irqs();

/**
 * Make interrupt service routines take a pointer to the registers as a parameter
 */
typedef void (*isr_t)(registers_t*);

/**
 * Register an interrupt handler at an specified index
 * @param n index into the interrupt handler table
 * @param handler
 */
void register_interrupt_handler(uint8_t n, isr_t handler);

extern void* syscall_stub;

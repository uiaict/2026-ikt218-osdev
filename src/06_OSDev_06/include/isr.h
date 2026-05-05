#pragma once

#include <libc/stdint.h>

/*
 * registers_t - CPU state snapshot saved by the ISR common stub
 *
 * The assembly stub pushes registers onto the stack in a specific order
 * before calling isr_dispatch(). This struct maps to that exact layout.
 * The C handler receives a pointer to this struct (= the stack pointer
 * value at the moment of the call).
 *
 * Push order in interrupt_stubs.asm (last push = lowest address = first field):
 *
 *   CPU (automatic):    EFLAGS, CS, EIP         ← highest addresses
 *   stub macro:         err_code, int_no
 *   isr_common_stub:    pusha (EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI → top=EDI)
 *   isr_common_stub:    push ds                 ← lowest address (ESP here)
 *
 * So from LOW address to HIGH address (struct field order):
 *   ds, edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax,
 *   int_no, err_code, eip, cs, eflags
 */
typedef struct {
    uint32_t ds;         /* Saved data segment (pushed after pusha)     */
    uint32_t edi;        /* \                                            */
    uint32_t esi;        /*  |                                           */
    uint32_t ebp;        /*  | Saved by pusha (pusha pushes EAX first,  */
    uint32_t esp_dummy;  /*  |  EDI last → EDI is at lowest address)    */
    uint32_t ebx;        /*  |                                           */
    uint32_t edx;        /*  |                                           */
    uint32_t ecx;        /*  |                                           */
    uint32_t eax;        /* /                                            */
    uint32_t int_no;     /* Interrupt vector number (pushed by stub)     */
    uint32_t err_code;   /* Error code (CPU or dummy 0 from stub)        */
    uint32_t eip;        /* \                                            */
    uint32_t cs;         /*  | Pushed automatically by the CPU           */
    uint32_t eflags;     /* /                                            */
} __attribute__((packed)) registers_t;

/* Signature for a C-level ISR handler */
typedef void (*isr_handler_t)(registers_t *regs);

/*
 * isr_install_handler - register a C handler for a CPU exception (0-31)
 * @isr_num : exception vector (0 = divide error, 14 = page fault, etc.)
 * @handler : function to call when this exception fires
 */
void isr_install_handler(uint8_t isr_num, isr_handler_t handler);

/*
 * isr_dispatch - called from isr_common_stub in interrupt_stubs.asm
 * Dispatches to a registered handler or prints a default error message.
 */
void isr_dispatch(registers_t *regs);

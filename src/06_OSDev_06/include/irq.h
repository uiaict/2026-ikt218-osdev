#pragma once

#include <isr.h>

/* Signature for a C-level IRQ handler (same register frame as ISR) */
typedef void (*irq_handler_t)(registers_t *regs);

/*
 * irq_init - remap the 8259 PIC and clear the IRQ handler table
 *
 * By default the PIC maps IRQ0-7 to INT 0x08-0x0F, which conflicts
 * with CPU exception vectors. This remaps them to 0x20-0x2F.
 */
void irq_init(void);

/*
 * irq_install_handler - register a C handler for a hardware IRQ line
 * @irq_num : IRQ line number (0-15); IRQ1 = PS/2 keyboard
 * @handler : function called when this IRQ fires
 */
void irq_install_handler(uint8_t irq_num, irq_handler_t handler);

/*
 * irq_dispatch - called from irq_common_stub in interrupt_stubs.asm
 * Dispatches to the registered handler then sends EOI to the PIC.
 */
void irq_dispatch(registers_t *regs);

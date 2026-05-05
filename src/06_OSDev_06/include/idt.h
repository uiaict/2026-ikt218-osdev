#pragma once

#include <libc/stdint.h>

/*
 * IDT Entry (Gate Descriptor) - 8 bytes
 *
 * Describes a single interrupt vector: where to jump (base_low/high),
 * which code segment to use (selector), and what kind of gate (flags).
 *
 * Bit layout:
 *   [15: 0] base_low   - bits 15:0  of the handler address
 *   [31:16] selector   - code segment selector (must be 0x08: kernel CS)
 *   [39:32] always0    - reserved, must be zero
 *   [47:40] flags      - type and attribute byte (see below)
 *   [63:48] base_high  - bits 31:16 of the handler address
 *
 * Flags byte (0x8E for a 32-bit interrupt gate at ring 0):
 *   Bit 7   (P)    : Present           - 1 = entry is valid
 *   Bits 6:5 (DPL) : Privilege level   - 0 = kernel only
 *   Bit 4          : always 0 for interrupt/trap gates
 *   Bits 3:0 (Type): Gate type
 *     0xE = 32-bit Interrupt Gate (clears IF on entry)
 *     0xF = 32-bit Trap Gate      (leaves IF unchanged)
 *
 * We use 0x8E (interrupt gate) for all entries so interrupts are
 * automatically disabled on handler entry, preventing re-entrancy issues.
 */
typedef struct {
    uint16_t base_low;  /* Handler address bits 15:0  */
    uint16_t selector;  /* Code segment (0x08)        */
    uint8_t  always0;   /* Reserved, set to 0         */
    uint8_t  flags;     /* Type + attribute byte      */
    uint16_t base_high; /* Handler address bits 31:16 */
} __attribute__((packed)) idt_entry_t;

/*
 * IDT Pointer (IDTR register value)
 *
 * This 6-byte structure is what the lidt instruction reads.
 * Format is identical to the GDTR used by lgdt.
 */
typedef struct {
    uint16_t limit; /* Size of IDT in bytes - 1  */
    uint32_t base;  /* Linear address of the IDT */
} __attribute__((packed)) idt_ptr_t;

/* Initialise all 256 IDT entries and load the IDTR. */
void idt_init(void);

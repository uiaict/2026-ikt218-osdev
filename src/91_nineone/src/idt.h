#ifndef IDT_H
#define IDT_H

#include "libc/stdint.h"

#define IDT_MAX_DESCRIPTORS 256


typedef struct {
	uint16    isr_low;      // The lower 16 bits of the ISR's address
	uint16    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8     reserved;     // Set to zero
	uint8     attributes;   // Type and attributes; see the IDT page
	uint16    isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;





typedef struct {
	uint16    limit;
	uint32    base;
} __attribute__((packed)) idtr_t;

void idt_init(void);




void idt_set_descriptor(uint8 vector, void* isr, uint8 flags);


void idt_enable_interrupts(void);



#endif
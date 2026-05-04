#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <libc/stdint.h>

#define IDT_ENTRIES 256

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t   idt_ptr;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = (uint16_t)(base & 0xFFFF);
    idt[num].base_hi = (uint16_t)((base >> 16) & 0xFFFF);
    idt[num].sel     = sel;
    idt[num].always0 = 0;
    idt[num].flags   = flags;
}

void idt_init(void) {
    idt_ptr.limit = (uint16_t)(sizeof(idt_entry_t) * IDT_ENTRIES - 1);
    idt_ptr.base  = (uint32_t)&idt;

    uint16_t cs;
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));

    for (int i = 0; i < IDT_ENTRIES; i++)
        idt_set_gate((uint8_t)i, 0, cs, 0);

    isr_init(cs);
    irq_init(cs);

    __asm__ volatile ("lidt %0" : : "m"(idt_ptr));
}

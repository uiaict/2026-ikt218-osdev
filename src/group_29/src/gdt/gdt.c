#include "gdt.h"

struct gdt_entry create_gdt_entry(
    uint32_t base, 
    uint32_t limit, 
    uint8_t access_byte, 
    uint8_t flags) {
    struct gdt_entry r;
    
    // Store lowest 16 bits of base address
    r.base_low = base & 0xFFFF;

    // Store middle 8 bits of base address
    r.base_middle = (base >> 16) & 0xFF;

    // Store highest 8 bits of base address
    r.base_high = (base >> 24) & 0xFF;


    // Store lowest 16 bits of segment limit
    r.limit_low = limit & 0xFFFF;


    // Store the high 4 bits of the segment limit
    // (limit bits 16–19)
    r.granularity = (limit >> 16) & 0x0F;
    r.granularity |= (flags & 0xF0);

    return r;
}

struct gdt_pointer create_gdt_pointer(struct gdt_entry *start, uint32_t entries)
{
    struct gdt_pointer r;
    r.address = (uint32_t)start;
    r.limit = (entries * 8)-1;
    return r;
}

void gdt_init() {
    struct gdt_entry e[3];
    e[0] = create_gdt_entry(0,0,0,0);
    struct gdt_pointer p = create_gdt_pointer(e,3);
    struct gdt_pointer* pp = &p;

    asm volatile (
        ".intel_syntax noprefix\n\t"
        "lgdt [%0]\n\t"
        "mov ax, 0x10\n\t"
        "mov ds, ax\n\t"
        "mov es, ax\n\t"
        "mov fs, ax\n\t"
        "mov gs, ax\n\t"
        "mov ss, ax\n\t"
        "push 0x08\n\t"       // Push code selector
        "push offset 1f\n\t"  // Push address of label 1
        "retf\n\t"            // Far return to reload CS
        "1:\n\t"
        ".att_syntax\n\t"
        : : "r" (&p) : "ax"
    );


}

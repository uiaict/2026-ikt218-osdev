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
        // Some asm from here http://www.osdever.net/bkerndev/Docs/gdt.htm
        ".intel_syntax noprefix  \n\t" // Switch to Intel/NASM style
        "lgdt [%0]               \n\t" // NASM-style addressing
        "mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment\n\t"
        "mov ds, ax\n\t"
        "mov es, ax\n\t"
        "mov fs, ax\n\t"
        "mov gs, ax\n\t"
        "mov ss, ax\n\t"
        "jmp 0x08:flush2   ; 0x08 is the offset to our code segment: Far jump!\n\t"
        "flush2:\n\t"
        "ret               ; Returns back to the C code!\n\t"
        ".att_syntax             \n\t" // Switch back to keep the compiler happy
        :
        : "r" (p)                // Pass the pointer in a register
    );


}

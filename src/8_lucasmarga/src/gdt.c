#include "gdt.h"
#include "libc/stdint.h"

/*
 * Each GDT entry is 8 bytes in size, as defined by the hardware.
 * This structure follows the 32-bit x86 descriptor format. 
 */
typedef struct __attribute__((packed)) {
    uint16_t limit_low;      
    uint16_t base_low;      
    uint8_t  base_mid;       
    uint8_t  access;         
    uint8_t  granularity;    
    uint8_t  base_high;      
} gdt_descriptor;

/*
 * The structure is used by the lgdt instruction to load the GDT.
 * The limit specifies the total size of the GDT in bytes minus one. 
 * The base stores the memory address of the first entry in the table.
 */
typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint32_t base;
} gdt_register;

/* The GDT contains three entries: 0 = NULL, 1 = Code segment, and 2 = Data segment*/
static gdt_descriptor gdt[3];
static gdt_register   gdtr;

/*
 * Implemented in the gdt_flush.asm:
 * - Loads the GDT using lgdt [gdtr]
 * - Reloads all segment registers
 * - Performs a far jump to update the CS register
 */
extern void gdt_load(const gdt_register* gdtr);

/*
 * Helper function that sets up a single GDT entry using the given 
 * base address, limit, access byte, and flags. 
 *
 * For a flat 4 GiB segment, the limit is 0xFFFFF (20-bit).
 * 
 * The flags usually include:
 *   - 0x80: Enables 4 KiB granularity  (G=1)
 *   - 0x40: Selects a 32-bit segment      (D=1)
 * 
 * When combined, these flags result in 0xC0
 */
static void gdt_create_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt[index].limit_low = (uint16_t)(limit & 0xFFFF);
    gdt[index].base_low  = (uint16_t)(base & 0xFFFF);
    gdt[index].base_mid  = (uint8_t)((base >> 16) & 0xFF);
    gdt[index].access    = access;

    gdt[index].granularity = (uint8_t)((limit >> 16) & 0x0F);

    gdt[index].granularity |= (uint8_t)(flags & 0xF0);

    gdt[index].base_high = (uint8_t)((base >> 24) & 0xFF);
}

void gdt_setup(void) {
    /*
     * Entry 0: NULL descriptor 
     * This entry is required and must be set to zero. 
     */
    gdt_create_entry(0, 0, 0, 0, 0);

    /*
     * Flat memory model (base=0, limit = 4 GiB):
     * The limit is set to 0xFFFFF, and with 4 KiB granularity enabled,
     * this corresponds to approx. 4 GiB of addressable space. 
     *
     * Access byte:
     *  - Code: 0x9A = present, ring0, executable code segment, readable
     *  - Data: 0x92 = present, ring0, data segment, writable
     *
     * Flags:
     *  - 0xC0 = G=1 (4 KiB granularity), D=1 (32-bit)
     */
    gdt_create_entry(1, 0x00000000, 0x000FFFFF, 0x9A, 0xC0); 
    gdt_create_entry(2, 0x00000000, 0x000FFFFF, 0x92, 0xC0); 

    /*
     * Set up the GDTR and load it using the assembly routine.
     */
    gdtr.limit = (uint16_t)(sizeof(gdt) - 1);
    gdtr.base  = (uint32_t)&gdt[0];

    gdt_load(&gdtr);
}
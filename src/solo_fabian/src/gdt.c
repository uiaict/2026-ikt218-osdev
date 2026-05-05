/* Public GDT declarations and fixed-width integer types. */
#include <gdt.h>

/* This kernel starts with exactly three descriptors: null, code, and data. */
#define GDT_ENTRY_COUNT 3

/* Present, ring 0, executable, readable code segment. */
#define GDT_ACCESS_CODE 0x9A

/* Present, ring 0, writable data segment. */
#define GDT_ACCESS_DATA 0x92

/* 4 KiB granularity and 32-bit protected-mode segment size. */
#define GDT_FLAGS_32BIT_GRANULAR 0xCF

/* One packed 8-byte x86 GDT descriptor in the exact layout the CPU expects. */
struct gdt_entry {
    /* Lower 16 bits of the segment limit. */
    uint16_t limit_low;

    /* Lower 16 bits of the segment base address. */
    uint16_t base_low;

    /* Middle 8 bits of the segment base address. */
    uint8_t base_middle;

    /* Access byte: present bit, privilege level, descriptor type, and access rights. */
    uint8_t access;

    /* Upper 4 limit bits plus flags such as granularity and 32-bit mode. */
    uint8_t granularity;

    /* Highest 8 bits of the segment base address. */
    uint8_t base_high;
} __attribute__((packed));

/* The small pointer structure consumed by the lgdt instruction. */
struct gdt_pointer {
    /* Size of the whole GDT minus one, as required by lgdt. */
    uint16_t limit;

    /* Linear address where the GDT begins. */
    uint32_t base;
} __attribute__((packed));

/* Assembly helper in gdt.asm that executes lgdt and reloads segment registers. */
extern void gdt_load(struct gdt_pointer* gdt_pointer);

/* Storage for the actual GDT descriptors. */
static struct gdt_entry gdt[GDT_ENTRY_COUNT];

/* Storage for the pointer passed to lgdt. */
static struct gdt_pointer gdt_ptr;

/* Fill one descriptor in the table from normal base, limit, access, and flag values. */
static void gdt_set_entry(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    /* Store bits 0..15 of the segment limit. */
    gdt[index].limit_low = (uint16_t)(limit & 0xFFFF);

    /* Store bits 0..15 of the base address. */
    gdt[index].base_low = (uint16_t)(base & 0xFFFF);

    /* Store bits 16..23 of the base address. */
    gdt[index].base_middle = (uint8_t)((base >> 16) & 0xFF);

    /* Store the access byte exactly as the CPU will read it. */
    gdt[index].access = access;

    /* Store limit bits 16..19 in the low nibble and flags in the high nibble. */
    gdt[index].granularity = (uint8_t)(((limit >> 16) & 0x0F) | (flags & 0xF0));

    /* Store bits 24..31 of the base address. */
    gdt[index].base_high = (uint8_t)((base >> 24) & 0xFF);
}

/* Create and load the kernel's initial flat GDT. */
void gdt_init(void)
{
    /* lgdt wants the table size minus one, not the plain byte size. */
    gdt_ptr.limit = (uint16_t)(sizeof(gdt) - 1);

    /* Point lgdt at the first byte of the descriptor array. */
    gdt_ptr.base = (uint32_t)&gdt;

    /*
     * Limit 0xFFFFF with 4 KiB granularity covers the whole 4 GiB
     * 32-bit address space, which is the usual flat protected-mode setup.
     */
    /* Entry 0 must be all zeroes; loading selector 0 is invalid on purpose. */
    gdt_set_entry(0, 0, 0, 0, 0);

    /* Entry 1 is the kernel code/text segment at selector 0x08. */
    gdt_set_entry(1, 0, 0xFFFFF, GDT_ACCESS_CODE, GDT_FLAGS_32BIT_GRANULAR);

    /* Entry 2 is the kernel data segment at selector 0x10. */
    gdt_set_entry(2, 0, 0xFFFFF, GDT_ACCESS_DATA, GDT_FLAGS_32BIT_GRANULAR);

    /* Load the new table into GDTR and refresh the CPU's cached segment state. */
    gdt_load(&gdt_ptr);
}

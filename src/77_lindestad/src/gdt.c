#include <gdt.h>

#include <libc/stdint.h>

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_pointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

enum {
    GDT_ENTRY_COUNT = 3,
};

static struct gdt_entry gdt_entries[GDT_ENTRY_COUNT];
static struct gdt_pointer gdt_ptr;

extern void gdt_flush(uint32_t gdt_ptr_address);

static void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    gdt_entries[index].base_low = (uint16_t)(base & 0xFFFF);
    gdt_entries[index].base_middle = (uint8_t)((base >> 16) & 0xFF);
    gdt_entries[index].base_high = (uint8_t)((base >> 24) & 0xFF);

    gdt_entries[index].limit_low = (uint16_t)(limit & 0xFFFF);
    gdt_entries[index].granularity = (uint8_t)((limit >> 16) & 0x0F);
    gdt_entries[index].granularity |= granularity & 0xF0;

    gdt_entries[index].access = access;
}

void gdt_initialize(void)
{
    gdt_ptr.limit = (uint16_t)(sizeof(gdt_entries) - 1);
    gdt_ptr.base = (uint32_t)&gdt_entries;

    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xCF);
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xCF);

    gdt_flush((uint32_t)&gdt_ptr);
}

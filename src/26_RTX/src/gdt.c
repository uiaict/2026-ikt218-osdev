#include <gdt.h>

extern void gdt_flush(uint32_t gdt_ptr_addr);

static struct gdt_entry gdt_entries[3];
static struct gdt_ptr gdt_descriptor;

static void gdt_set_gate(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt_entries[index].base_low = (uint16_t)(base & 0xFFFFU);
    gdt_entries[index].base_middle = (uint8_t)((base >> 16) & 0xFFU);
    gdt_entries[index].base_high = (uint8_t)((base >> 24) & 0xFFU);

    gdt_entries[index].limit_low = (uint16_t)(limit & 0xFFFFU);
    gdt_entries[index].granularity = (uint8_t)((limit >> 16) & 0x0FU);
    gdt_entries[index].granularity |= (uint8_t)(granularity & 0xF0U);

    gdt_entries[index].access = access;
}

void gdt_init(void) {
    gdt_descriptor.limit = (uint16_t)(sizeof(gdt_entries) - 1U);
    gdt_descriptor.base = (uint32_t)&gdt_entries;

    gdt_set_gate(0, 0U, 0U, 0U, 0U);                /* Null descriptor */
    gdt_set_gate(1, 0U, 0x000FFFFFU, 0x9AU, 0xCFU); /* Kernel code */
    gdt_set_gate(2, 0U, 0x000FFFFFU, 0x92U, 0xCFU); /* Kernel data */

    gdt_flush((uint32_t)&gdt_descriptor);
}

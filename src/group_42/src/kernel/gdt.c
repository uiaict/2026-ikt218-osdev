#include "kernel/gdt.h"

#include "libc/stdio.h"


// NOTE: https://wiki.osdev.org/GDT_Tutorial

gdt_entry_t gdt[GDT_ENTRIES];
gdt_ptr_t gdt_ptr;



void init_gdt() {
  // set gdt limit
  printf("Initialising Global Descriptor Table...\n", GDT_ENTRIES);
  gdt_ptr.limit = sizeof(gdt_entry_t) * GDT_ENTRIES - 1;
  gdt_ptr.base = (uint32_t)&gdt;

  gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment


  // load gdt
  gdt_load(&gdt_ptr);

  // flush gdt pointer
  // gdt_flush((uint32_t)&gdt_ptr);
  printf("Successfully Initialised Global Descriptor Table.\n");
}
void gdt_load(gdt_ptr_t* gdt_ptr) {
  asm volatile("lgdt %0" : : "m"(*gdt_ptr));
}

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access,
                   uint8_t granularity) {
  gdt[num].base_low = (base & 0xFFFF);
  gdt[num].base_middle = (base >> 16) & 0xFF;
  gdt[num].base_high = (base >> 24) & 0xFF;

  gdt[num].limit_low = (limit & 0xFFFF);
  gdt[num].granularity = (limit >> 16) & 0x0F;

  gdt[num].granularity |= granularity & 0xF0;
  gdt[num].access = access;
}

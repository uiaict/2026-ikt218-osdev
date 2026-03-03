#include "arch/i386/cpu/idt.h"

#include <stdio.h>

#include "arch/i386/cpu/isr.h"


void set_idt_gate(int n, uint32_t handler) {
  idt[n].low_offset = handler & 0xFFFF;
  idt[n].kernel_cs = KERNEL_CS;
  idt[n].reserved = 0;
  idt[n].attributes = 0x8E;
  idt[n].high_offset = (handler >> 16) & 0xFFFF;
}

void init_idt() {
  // set idt limit
  idt_reg.base = (uintptr_t)&idt[0];
  idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;

  init_isrs();
  init_irqs();

  __asm__ volatile("lidtl (%0)" : : "r"(&idt_reg)); // load new IDT
  printf("Initialized IDT.\n");
}

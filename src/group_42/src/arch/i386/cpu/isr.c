#include "arch/i386/cpu/isr.h"

#include <kernel/pit.h>
#include <stdio.h>

#include "arch/i386/cpu/idt.h"
#include "arch/i386/cpu/ports.h"
#include "drivers/input/keyboard.h"
#include "kernel/util.h"


static isr_t interrupt_handlers[256];
extern uint32_t isr_stub_table[ISR_CPU_COUNT];
extern uint32_t irq_stub_table[IRQ_COUNT];

static const char* const exception_names[] = {"Divide Error",
                                              "Debug",
                                              "NMI Interrupt",
                                              "Breakpoint",
                                              "Overflow",
                                              "BOUND Range Exceeded",
                                              "Invalid Opcode",
                                              "Device Not Available",
                                              "Double Fault",
                                              "Coprocessor Segment Overrun",
                                              "Invalid TSS",
                                              "Segment Not Present",
                                              "Stack-Segment Fault",
                                              "General Protection Fault",
                                              "Page Fault",
                                              "x87 Floating-Point",
                                              "Alignment Check",
                                              "Machine Check",
                                              "SIMD Floating-Point",
                                              "Virtualization",
                                              "Reserved 20",
                                              "Reserved 21",
                                              "Reserved 22",
                                              "Reserved 23",
                                              "Reserved 24",
                                              "Reserved 25",
                                              "Reserved 26",
                                              "Reserved 27",
                                              "Reserved 28",
                                              "Reserved 29",
                                              "Reserved 30",
                                              "Reserved 31"};

// Called from assembly stubs
void isr_handler(registers_t* regs) {
  printf("Received interrupt: ");
  char s[3];
  int_to_ascii(regs->int_no, s);
  printf(s);
  printf("\n");
  printf(exception_names[regs->int_no]);
  printf("\n");
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
  interrupt_handlers[n] = handler;
}


void irq_handler(registers_t* regs) {
  if (regs->int_no >= 40)
    port_byte_out(0xA0, 0x20); // slave
  port_byte_out(0x20, 0x20);   // master

  if (interrupt_handlers[regs->int_no] != 0) {
    isr_t handler = interrupt_handlers[regs->int_no];
    handler(regs);
  } else {
    printf("Unhandled IRQ: ");
    char s[4];
    int_to_ascii(regs->int_no, s);
    printf(s);
    printf("\n");
  }
}


void init_irqs() {
  __asm__ volatile("sti");
  init_pit();
  init_keyboard();
}

void init_isrs() {
  // Install CPU ISRs
  for (int i = 0; i < ISR_CPU_COUNT; i++) {
    set_idt_gate(i, isr_stub_table[i]);
  }

  // Remap PIC
  port_byte_out(0x20, 0x11);
  port_byte_out(0xA0, 0x11);
  port_byte_out(0x21, 0x20);
  port_byte_out(0xA1, 0x28);
  port_byte_out(0x21, 0x04);
  port_byte_out(0xA1, 0x02);
  port_byte_out(0x21, 0x01);
  port_byte_out(0xA1, 0x01);
  port_byte_out(0x21, 0x0);
  port_byte_out(0xA1, 0x0);

  // Install IRQs
  for (int i = 0; i < 16; i++) {
    set_idt_gate(i + IRQ0, irq_stub_table[i]);
  }

  // syscalls
  uint32_t handler = (uint32_t)syscall_stub;
  idt_gate_t* gate = &idt[0x80];
  gate->low_offset = handler & 0xFFFF;
  gate->kernel_cs = KERNEL_CS;
  gate->reserved = 0;
  gate->attributes = 0xEE; // user access
  gate->high_offset = (handler >> 16) & 0xFFFF;
}

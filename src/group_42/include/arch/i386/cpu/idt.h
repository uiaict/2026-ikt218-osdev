#pragma once

#define IDT_ENTRIES 256
#define KERNEL_CS 0x08


#include <libc/stdint.h>


typedef struct __attribute__((packed)) {
  uint16_t low_offset; // Lower 16 bits of ISR address
  uint16_t kernel_cs;  // GDT segment selector
  uint8_t reserved;    // Set to zero
  uint8_t attributes;
  uint16_t high_offset; // Upper 16 bits of ISR address
} idt_gate_t;

typedef struct __attribute__((packed)) {
  uint16_t limit;
  uint32_t base;
} idtr_t;

typedef struct __attribute__((packed)) {
  uint16_t num;
  void* handler;
  void* data;
} int_handler_t;

static idt_gate_t idt[IDT_ENTRIES];
static idtr_t idt_reg;


void init_idt();
void set_idt_gate(int n, uint32_t handler);

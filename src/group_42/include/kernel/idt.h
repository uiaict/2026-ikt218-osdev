#pragma once
#define IDT_ENTRIES 256
#include <libc/stdint.h>


typedef struct __attribute__((packed)) {
  uint16_t base_low;
  uint16_t selector;
  uint8_t zero;
  uint8_t flags;
  uint16_t base_high;
} idt_entry_t;


typedef struct __attribute__((packed)) {
  uint16_t limit;
  uint32_t base;
} idt_ptr_t;

typedef struct __attribute__((packed)) {
  // TODO: What datatype is this
  uint16_t num;
  void* handler;
  void* data;
} int_handler_t;

void init_idt();
void idt_load(idt_ptr_t* idt_ptr);
void interrupt_handler();

#pragma once

#include <libc/stdint.h>

struct interrupt_registers {
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t interrupt_number;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

typedef void (*interrupt_handler_t)(struct interrupt_registers* registers);

void interrupts_initialize(void);
void interrupt_register_handler(uint8_t interrupt_number, interrupt_handler_t handler);
void interrupt_handler(struct interrupt_registers* registers);
void keyboard_set_echo(uint8_t enabled);
uint8_t keyboard_has_key(void);
char keyboard_read_char(void);

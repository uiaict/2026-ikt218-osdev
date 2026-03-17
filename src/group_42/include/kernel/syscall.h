#pragma once
#include <arch/i386/cpu/isr.h>
#include <stdint.h>

#define SYS_read 3
#define SYS_write 4
#define SYS_open 5
#define SYS_close 6
#define SYS_exit 1
#define SYS_fork 2
#define SYS_getpid 20
#define SYS_brk 45

#define MAX_SYSCALLS 64

/**
 * Initialize the syscall table and register the interrupt handler
 */
void init_syscalls();

/**
 * The syscall handler
 */
void syscall_handler(registers_t* regs);

/**
 * Registers a new syscall
 * @param num index into syscall table
 * @param handler handler for syscall
 */
void register_syscall(uint32_t num, uint32_t (*handler)());

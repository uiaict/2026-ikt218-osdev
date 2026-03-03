#include "kernel/syscall.h"

#include <stddef.h>

#include "arch/i386/cpu/isr.h"
#include "arch/i386/cpu/ports.h"

static uint32_t (*syscall_table[MAX_SYSCALLS])();

static uint32_t sys_write(uint32_t fd, uint32_t buf, uint32_t count) {
  const char* str = (const char*)buf;
  for (uint32_t i = 0; i < count && str[i]; i++) {
    port_byte_out(0xE9, str[i]); // Write to QEMU debug port
  }
  return count;
}
static uint32_t sys_getpid() {
  return 1; // First process
}
static uint32_t sys_exit(uint32_t status) {
  // Halt the system (or implement proper process termination)
  for (;;)
    __asm__ volatile("hlt");
  return 0;
}


uint32_t last_syscall_return = 0;

void syscall_handler(registers_t* regs) {
  uint32_t syscall_num = regs->eax;
  uint32_t result = -1;

  if (syscall_num < MAX_SYSCALLS && syscall_table[syscall_num] != NULL) {
    result = syscall_table[syscall_num](&regs);
  }

  regs->eax = result;
}

void register_syscall(uint32_t num, uint32_t (*handler)()) {
  if (num < MAX_SYSCALLS) {
    syscall_table[num] = handler;
  }
}

void init_syscalls() {
  for (int i = 0; i < MAX_SYSCALLS; i++) {
    syscall_table[i] = NULL;
  }

  register_syscall(SYS_write, sys_write);
  register_syscall(SYS_getpid, sys_getpid);
  register_syscall(SYS_exit, sys_exit);

  register_interrupt_handler(0x80, (isr_t)syscall_stub);
}

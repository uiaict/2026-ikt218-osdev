#include <drivers/video/vga_text.h>
#include <kernel/errno.h>
#include <kernel/fcntl.h>
#include <kernel/syscall.h>
#include <kernel/unistd.h>
#include <stdio.h>
#include <string.h>

#include "shell/shell_command.h"

int cmd_test_syscalls(int argc, char** argv) {
  (void)argc;
  (void)argv;
  syscall_args_t args;

  printf("=== SYS_WRITE TEST (fd=1 stdout) ===\n");
  args = (syscall_args_t){0, .a = 1, .b = (uint32_t)"hello from sys_write\n", .c = 20};
  uint32_t written = sys_write(&args);
  printf("sys_write returned: %u (expected 20)\n", written);

  printf("=== SYS_WRITE TEST (fd=99 invalid) ===\n");
  args = (syscall_args_t){0, .a = 99, .b = (uint32_t)"x", .c = 1};
  written = sys_write(&args);
  printf("sys_write (fd=99) returned: %d (expected -EBADF=%d)\n", written, -EBADF);

  printf("=== SYS_READ TEST (fd=99 invalid) ===\n");
  uint8_t tmpbuf[4] = {0};
  args = (syscall_args_t){0, .a = 99, .b = (uint32_t)tmpbuf, .c = 4};
  uint32_t bad_read = sys_read(&args);
  printf("sys_read (fd=99) returned: %d (expected -EBADF=%d)\n", bad_read, -EBADF);

  printf("=== SYS_GETPID TEST ===\n");
  args = (syscall_args_t){0};
  uint32_t pid = sys_getpid(&args);
  printf("sys_getpid returned: %u (expected 1)\n", pid);

  printf("=== SYS_GETUID TEST ===\n");
  args = (syscall_args_t){0};
  uint32_t uid = sys_getuid(&args);
  printf("sys_getuid returned: %u (expected 0)\n", uid);

  printf("=== SYS_GETGID TEST ===\n");
  args = (syscall_args_t){0};
  uint32_t gid = sys_getgid(&args);
  printf("sys_getgid returned: %u (expected 0)\n", gid);

  printf("=== SYS_GETCWD TEST ===\n");
  char cwd_buf[8] = {0};
  args = (syscall_args_t){0, .a = (uint32_t)cwd_buf, .b = 8};
  uint32_t cwd_ret = sys_getcwd(&args);
  printf("sys_getcwd returned: 0x%x\n", cwd_ret);
  printf("sys_getcwd buffer: \"%s\" (expected \"/\")\n", cwd_buf);

  printf("=== SYS_GETCWD TEST (small buffer) ===\n");
  char tiny_buf[1] = {0};
  args = (syscall_args_t){0, .a = (uint32_t)tiny_buf, .b = 1};
  cwd_ret = sys_getcwd(&args);
  printf("sys_getcwd (size=1) returned: %d (expected -EINVAL=%d)\n", cwd_ret, -EINVAL);

  printf("=== STUB SYSCALLS (all return -1) ===\n");
  args = (syscall_args_t){0};
  printf("sys_fork returned: %d\n", sys_fork(&args));
  printf("sys_execve returned: %d\n", sys_execve(&args));
  printf("sys_wait returned: %d\n", sys_wait(&args));
  printf("sys_open returned: %d\n", sys_open(&args));
  printf("sys_lseek returned: %d\n", sys_lseek(&args));
  printf("sys_dup returned: %d\n", sys_dup(&args));
  printf("sys_dup2 returned: %d\n", sys_dup2(&args));
  printf("sys_pipe returned: %d\n", sys_pipe(&args));
  printf("sys_chdir returned: %d\n", sys_chdir(&args));

  printf("=== SYS_READ TEST (fd=0 keyboard) ===\n");
  printf("Press 5 keys then press Enter...\n");
  uint8_t kbuf[8] = {0};
  args = (syscall_args_t){0, .a = 0, .b = (uint32_t)kbuf, .c = 5};
  uint32_t bytes_read = sys_read(&args);
  printf("sys_read returned: %u bytes\n", bytes_read);
  printf("sys_read bytes: '");
  for (uint32_t i = 0; i < bytes_read; i++) {
    putchar(kbuf[i]);
  }
  printf("'\n");

  printf("=== ALL TESTS COMPLETE ===\n");
  return 0;
}

#include "shell/commands/clear.h"

#include "drivers/video/vga_terminal.h"

int cmd_clear(int argc, char** argv) {
  (void)argc;
  (void)argv;
  vga_clear_screen();
  return 0;
}

#include "shell/commands/help.h"

#include <stdio.h>
#include "shell/shell_command.h"

int cmd_help(int argc, char** argv) {
  (void)argc;
  (void)argv;
  for (size_t i = 0; i < NUM_COMMANDS; i++) {
    printf("%s\t\t%s\n", command_table[i].name, command_table[i].description);
  }
  return 0;
}

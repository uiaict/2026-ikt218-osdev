#include "shell/shell_command.h"

#include <drivers/video/vga_terminal.h>
#include <stdio.h>
#include <string.h>

#include "drivers/input/keyboard.h"


// TODO: we want to allocate memory for this, but we dont have memory management yet
#define MAX_ARGS 16
#define MAX_ARG_LEN 64

typedef int (*command_handler_t)(int argc, char** argv);


typedef struct {
  const char* name;
  const char* description;
  command_handler_t execute;
} shell_command_t;


int cmd_help(int argc, char** argv);
int cmd_clear(int argc, char** argv);
int cmd_echo(int argc, char** argv);
int cmd_keyboard_logger(int, char**);


static const shell_command_t command_table[] = {
    {           "help",  "Show this help message",            cmd_help},
    {          "clear",      "Clear the terminal",           cmd_clear},
    {           "echo",         "Print arguments",            cmd_echo},
    {"keyboard_logger", "Run the keyboard logger", cmd_keyboard_logger},
};
#define NUM_COMMANDS (sizeof(command_table) / sizeof(command_table[0]))


static char argv_static[MAX_ARGS][MAX_ARG_LEN];
static char* argv_ptrs[MAX_ARGS];

static int parse_args_fixed(const char* line, int* argc_out) {
  *argc_out = 0;
  if (!line || !*line)
    return -1;

  const char* p = line;
  int argi = 0;
  int arglen = 0;

  while (*p && argi < MAX_ARGS) {
    while (*p == ' ' || *p == '\t')
      p++;
    if (!*p)
      break;

    char* dest = argv_static[argi];
    argv_ptrs[argi] = dest;
    while (*p && *p != ' ' && *p != '\t' && arglen < MAX_ARG_LEN - 1) {
      *dest++ = *p++;
      arglen++;
    }
    *dest = '\0';
    arglen = 0;

    argi++;
    if (*p)
      p++;
  }

  *argc_out = argi;
  return (argi == 0 || argi >= MAX_ARGS) ? -1 : 0;
}


int execute_command(const char* line) {
  int argc = 0;
  if (parse_args_fixed(line, &argc) != 0) {
    printf("Parse error (max %d args)\n", MAX_ARGS);
    return -1;
  }

  if (argc == 0)
    return -1;

  const char* cmd_name = argv_ptrs[0];
  for (size_t i = 0; i < NUM_COMMANDS; i++) {
    if (strcmp(cmd_name, command_table[i].name) == 0) {
      return command_table[i].execute(argc, argv_ptrs);
    }
  }

  printf("Unknown command: %s\n", cmd_name);
  return -1;
}


int cmd_help(int argc, char** argv) {
  (void)argc;
  (void)argv;
  for (size_t i = 0; i < NUM_COMMANDS; i++) {
    printf("%s\t\t%s\n", command_table[i].name, command_table[i].description);
  }
  return 0;
}

int cmd_echo(int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    printf("%s%c", argv[i], (i + 1 < argc) ? ' ' : '\n');
  }
  if (argc <= 1)
    printf("\n");
  return 0;
}

int cmd_clear(int argc, char** argv) {
  (void)argc;
  (void)argv;
  vga_clear_screen();
  return 0;
}


int cmd_keyboard_logger(int argc, char** argv) {
  (void)argc;
  (void)argv;
  printf("Keyboard logger started. Type and press Enter; ESC or Ctrl+C exits.\n");

  while (1) {
    decode_keyboard();
    uint8_t key = 0;
    pop_key(&key);

    if (key != 0) {
      if (key == 27 || key == 3) { // ESC or ETX
        printf("\nLogger exited.\n");
        return 0;
      }
      printf("%c", key);
      fflush(stdout);
    }
  }
  return 0;
}

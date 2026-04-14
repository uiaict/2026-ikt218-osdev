#pragma once

#include "../string/string.h"
#include "../printing/printing.h"
#include "../libc/stdint.h"

#define MAX_ARGS 10

/// @brief Function pointer type for commands
typedef int (*CommandFunction)(int argument_count, char *arguments[]);

/// @brief Describes a shell command
typedef struct {
    const char* name;
    CommandFunction command_function;
} CommandEntry;

/// @brief Displays the shell prompt
void display_command_input(const char username[]);

/// @brief Parses input into arguments (argument_count/argument_count style)
int parse_command_aguments(char* input, char* arguments[], int max_args);

/// @brief Dispatches command to the correct handler
int handle_command(int argument_count, char* arguments[]);

/// @brief Parses and runs a command
///
/// @param input Non-const input string (it is changed while parsing)
///
/// @returns Result of the command, or negative on failure
int run_command(char* input);


/// @brief Print user input to screen
int command_echo(int argument_count, char* arguments[]);
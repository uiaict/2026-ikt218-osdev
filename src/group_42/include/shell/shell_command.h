#pragma once

/**
 * Executes a shell command from a provided line of text
 * @param line of text
 * @return -1 if failed to parse or too few arguments have bene provided
 */
int execute_command(const char* line);

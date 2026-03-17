#pragma once

/**
 * Initialize the shell, clearing the buffer, and disabling the hardware cursor
 */
void shell_init(void);

/**
 * Run the shell application, calls shell_init
 */
void shell_run(void);

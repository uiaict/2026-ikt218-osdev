#pragma once

/*
 * keyboard_init - register the PS/2 keyboard IRQ handler
 *
 * Must be called after irq_init() and before sti().
 * Installs a handler on IRQ1 that reads scancodes from port 0x60,
 * translates them to ASCII, and prints them to the terminal.
 */
void keyboard_init(void);

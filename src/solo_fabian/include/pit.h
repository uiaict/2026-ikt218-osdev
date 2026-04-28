#pragma once

#include <libc/stdint.h>

/* PIT I/O ports. */
#define PIT_CMD_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40

/* PIT runs at about 1.193 MHz. */
#define PIT_BASE_FREQUENCY 1193180

/* We want 1000 ticks per second, so one tick is about one millisecond. */
#define PIT_TARGET_FREQUENCY 1000

/* Divisor sent to PIT channel 0. */
#define PIT_DIVIDER (PIT_BASE_FREQUENCY / PIT_TARGET_FREQUENCY)

/* Command byte: channel 0, low/high byte, square wave mode. */
#define PIT_COMMAND 0x36

void init_pit(void);
void pit_handle_tick(void);
uint32_t pit_get_ticks(void); 
void sleep_busy(uint32_t milliseconds);
void sleep_interrupt(uint32_t milliseconds);


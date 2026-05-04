#pragma once

#include <libc/stdint.h>

#define PIT_CMD_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40
#define PIT_BASE_FREQUENCY 1193180
#define TARGET_FREQUENCY 1000
#define PIT_DIVIDER (PIT_BASE_FREQUENCY / TARGET_FREQUENCY)

void init_pit(void);
void sleep_interrupt(uint32_t milliseconds);
void sleep_busy(uint32_t milliseconds);
uint32_t pit_get_ticks(void);

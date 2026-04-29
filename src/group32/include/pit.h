#pragma once

#include "libc/stdint.h"

#define PIT_BASE_FREQUENCY 1193180

void init_pit(void);
uint32_t pit_get_ticks(void);
void pit_sleep_ms(uint32_t ms);
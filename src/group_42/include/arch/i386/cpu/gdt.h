#pragma once

#define GDT_ENTRIES 5

#include <libc/stdbool.h>

/**
 * Initialises global descriptor table
 * @return false if failed.
 */
bool init_gdt();

#pragma once

#define GDT_ENTRIES 5

#include <libc/stdbool.h>

// initializes global descriptor tables
bool init_gdt();

#pragma once
#include "libc/stdint.h"

/*
 * A Global Descriptor Table (GDT) with three entries will be installed: 
 *  Entry 0: a required NULL descriptor
 *  Entry 1: a code segment descriptor
 *  Entry 2: a data segment descriptor
 * 
 * The selectors, which are offsets into the GDT, are defined as: 
 * - Code selector: 0x08 (index 1 is shifted left by 3)
 * - Data selector: 0x10 (index 2 shifted left by 3)
*/

void gdt_setup(void); 
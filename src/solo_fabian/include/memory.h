#pragma once

#include <libc/stdint.h>

void init_kernel_memory(uint32_t* kernel_end);
void print_memory_layout(void);
void print_pointer(const char* label, void* pointer);
void* malloc(size_t size);
void free(void* pointer);
void init_paging(void);

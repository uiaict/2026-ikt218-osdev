#pragma once

#include <libc/stddef.h>
#include <libc/stdint.h>

typedef struct {
    uint8_t status;
    uint32_t size;
} alloc_t;

#ifdef __cplusplus
extern "C" {
#endif

void init_kernel_memory(uint32_t* kernel_end);
void init_paging(void);
void paging_map_virtual_to_phys(uint32_t virtual_address, uint32_t physical_address);
void print_memory_layout(void);

void* malloc(size_t size);
void free(void* memory);
char* pmalloc(size_t size);
void pfree(void* memory);

void* memcpy(void* dest, const void* src, size_t count);
void* memset(void* ptr, int value, size_t count);
void* memset16(void* ptr, uint16_t value, size_t count);

void test_new_operator(void);

#ifdef __cplusplus
}
#endif

/*
 * This is a header file for a simple memory management library.
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "libc/stdint.h"
#include "libc/stddef.h"

typedef struct {
    uint8_t status;
    uint32_t size;
} alloc_t;

/* Init Kernel Memory */
void init_kernel_memory(uint32_t* kernel_end);

/* Function declarations for paging operations */
extern void init_paging();
extern void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys);

/* Function declarations for memory allocation */
extern char* pmalloc(size_t size);
extern void* malloc(size_t size);
extern void free(void *mem);

/* Function declarations for memory manipulation */
extern void* memcpy(void* dest, const void* src, size_t num);
extern void* memset(void * ptr, int value, size_t num);
extern void* memset16(void *ptr, uint16_t value, size_t num);

/* Other helper functions*/
void print_memory_layout();

#endif
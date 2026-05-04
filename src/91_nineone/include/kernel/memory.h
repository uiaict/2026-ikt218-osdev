#ifndef MEMORY_H
#define MEMORY_H

#include <libc/stdint.h>
#include <libc/stddef.h>
#include "memory.h"



typedef struct {
    uint8 status;
    uint32 size;
} alloc_t;

void init_kernel_memory(uint32* kernel_end);

void print_memory_layout();

extern void* memcpy(void* dest, const void* src, size_t num ); /* Copies num bytes from src to dest */
extern void* memset (void * ptr, int value, size_t num ); /* Sets num bytes starting from ptr to value */
extern void* memset16 (void *ptr, uint16 value, size_t num); /* Sets num bytes starting from ptr to a 16-bit value */

extern char* pmalloc(size_t size); /* Allocates memory of given size with page alignment */
extern void* malloc(size_t size); /* Allocates memory of given size */
extern void free(void *mem); /* Frees memory previously allocated */

extern void init_paging(); /* Initializes paging */
extern void paging_map_virtual_to_phys(uint32 virt, uint32 phys); /* Maps a virtual address to a physical address */

#endif
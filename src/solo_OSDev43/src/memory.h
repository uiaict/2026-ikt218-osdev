#ifndef MEMORY_H
#define MEMORY_H

typedef unsigned int uint32_t;
typedef unsigned long size_t;
typedef unsigned char uint8_t;

typedef struct {
    uint8_t status;   // 1 = used, 0 = free
    uint32_t size;    // size of usable block
} alloc_t;

void init_kernel_memory(uint32_t* kernel_end);
void* malloc(size_t size);
void free(void* ptr);
void print_memory_layout(void);

#endif
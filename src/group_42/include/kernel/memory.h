#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct {
  uint8_t status;
  uint32_t size;
} alloc_t;

typedef struct {
  uint32_t last_alloc;
  uint32_t heap_end;
  uint32_t heap_begin;
  uint32_t memory_used;

  uint32_t pheap_begin;
  uint32_t pheap_end;
  uint8_t* pheap_desc;

} memory_info_t;

void init_mm(const uint32_t* kernel_end);

void init_paging();
void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys);

char* alloc_page();
void free_page(void* mem);


void* memset16(void* ptr, uint16_t value, size_t num);

void print_memory_layout();


// TODO: make arena allloocateors

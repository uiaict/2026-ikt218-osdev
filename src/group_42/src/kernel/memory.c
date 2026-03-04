#include "kernel/memory.h"

#include <assert.h>
#include <kernel/log.h>
#include <stddef.h>
#include <string.h>

#define MAX_PAGE_ALIGNED_ALLOCS 32

#define PAGE_TABLE_ENTRIES 1024
#define PAGE_SIZE 4096 // 4KB
#define PAGE_DIRECTORY_START 0x400000
#define PAGE_DIR_PHYSICAL_ADDR 0x404000
#define FIRST_PAGE_TABLE_ADDR 0x408000


memory_info_t memory_info;


// MUST be called before using memory_info
void init_mm(const uint32_t* kernel_end) {
  memory_info.last_alloc = (uint32_t)kernel_end + 0x1000;
  memory_info.heap_begin = memory_info.last_alloc;

  memory_info.pheap_end = PAGE_DIRECTORY_START;
  memory_info.pheap_begin = memory_info.pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * PAGE_SIZE);
  memory_info.heap_end = memory_info.pheap_begin;
  memset((char*)memory_info.heap_begin, 0, memory_info.heap_end - memory_info.heap_begin);
  memory_info.pheap_desc = (uint8_t*)malloc(MAX_PAGE_ALIGNED_ALLOCS);

  log_debug("Kernel heap starts at 0x%x\n", memory_info.last_alloc);
}

void print_memory_layout() {
  printf("Memory used: %d bytes\n", memory_info.memory_used);
  printf("Memory free: %d bytes\n",
         memory_info.heap_end - memory_info.heap_begin - memory_info.memory_used);
  printf("Heap size: %d bytes\n", memory_info.heap_end - memory_info.heap_begin);
  printf("Heap start: 0x%x\n", memory_info.heap_begin);
  printf("Heap end: 0x%x\n", memory_info.heap_end);
  printf("PHeap start: 0x%x\nPHeap end: 0x%x\n", memory_info.pheap_begin, memory_info.pheap_end);
}

// free, since pfree and pmalloc is nonstandard.
void free_page(void* mem) {
  if ((uint32_t)mem < memory_info.pheap_begin || (uint32_t)mem > memory_info.pheap_end)
    return;

  // Determine the page ID
  uint32_t ad = (uint32_t)mem;
  ad -= memory_info.pheap_begin;
  ad /= 4096;

  // Set the page descriptor to free
  memory_info.pheap_desc[ad] = 0;
  memory_info.memory_used -= PAGE_SIZE;
}


char* alloc_page() {
  // Loop through the available list
  for (int i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++) {
    if (memory_info.pheap_desc[i])
      continue;
    memory_info.pheap_desc[i] = 1;
    memory_info.memory_used += PAGE_SIZE;
    log_debug("PAllocated from 0x%x to 0x%x\n", memory_info.pheap_begin + i * PAGE_SIZE,
              memory_info.pheap_begin + (i + 1) * PAGE_SIZE);
    return (char*)(memory_info.pheap_begin + i * PAGE_SIZE);
  }
  log_debug("pmalloc: FATAL: failure!\n");
  return 0;
}

// paging


static uint32_t* page_directory = NULL;
static uint32_t* page_dir_loc = NULL;
static uint32_t* last_page = NULL;

// 0-8MB kernel stuff, heap from 1mb to 4mb, paging from 4mb

void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys) {
  uint16_t id = virt >> 22;
  for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
    last_page[i] = phys | 3; // present and write permissions
    phys += PAGE_SIZE;
  }
  assert(page_directory != NULL);
  page_directory[id] = ((uint32_t)last_page) | 3;             // present and write permissions
  last_page = (uint32_t*)(((uint32_t)last_page) + PAGE_SIZE); // next page
}
void paging_enable() {
  // clang-format off
  __asm__ volatile("mov %%eax, %%cr3": :"a"(page_dir_loc)); // Load the physical address of the page directory into the CR3 register
  __asm__ volatile("mov %cr0, %eax");         // Load the CR0 register into the EAX register
  __asm__ volatile("orl $0x80000000, %eax");  // Set the paging enable bit in the EAX register
  __asm__ volatile("mov %eax, %cr0");         // Load the EAX register into the CR0 register to enable paging
  // clang-format on
}

void init_paging() {
  log_info("Initializing paging...\n");

  page_directory = (uint32_t*)PAGE_DIR_PHYSICAL_ADDR;
  page_dir_loc = page_directory;
  last_page = (uint32_t*)FIRST_PAGE_TABLE_ADDR;
  for (int i = 0; i < 1024; i++) {
    page_directory[i] = 0 | 2;
  }
  paging_map_virtual_to_phys(0, 0);
  paging_map_virtual_to_phys(0x400000, 0x400000);
  paging_enable();

  log_debug("Paging was successfully enabled!\n");
}

void* memset16(void* ptr, uint16_t value, size_t num) {
  uint16_t* p = ptr;
  while (num--)
    *p++ = value; // Set each 2-byte element to the given value
  return ptr;
}

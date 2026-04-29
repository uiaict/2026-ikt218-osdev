#include "paging.h"
#include <stdint.h>

#define PAGE_PRESENT 0x1   // Page is present in memory
#define PAGE_RW      0x2   // Page is writable
#define PAGE_SIZE    0x80  // Page size flag (for 4MB pages, optional)

// Page directory and first page table, aligned to 4 KB
static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void init_paging() {
    // Initialize the first page table: map first 4 MB of memory 1:1
    for (int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | PAGE_PRESENT | PAGE_RW;
    }

    // Initialize the page directory: clear all entries first
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
    }

    // Point first page directory entry to our first page table
    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_RW;

    // Load page directory address into CR3 register
    asm volatile("mov %0, %%cr3" :: "r"(page_directory));

    // Enable paging by setting the paging bit in CR0
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Set the paging (PG) bit
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}
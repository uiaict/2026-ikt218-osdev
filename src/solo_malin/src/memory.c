#include <libc/stdint.h>
#include "memory.h"
#include "screen.h"

// ==============================
// Paging setup (32-bit, 4 KB)
// 
// Builds a simple page directory
// and identity-maps regions before
// enabling paging.
// ==============================

static uint32_t* page_directory = 0;  // Pointer to page directory
static uint32_t  page_dir_loc = 0;    // Physical address of page directory
static uint32_t* last_page = 0;       // Next free page table slot


// Map 4 MB of virtual space to 4 MB of physical space (identity map)
void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys){

    // Page directory index from top 10 bits of virtual address
    uint16_t id = virt >> 22;

    // Fill one page table (1024 entries → 4 MB)
    for (int i = 0; i < 1024; i++){
        // Present + writable (flags 0x3) | physical address
        last_page[i] = phys | 3;
        phys += 4096;
    }

    // Point directory entry to this page table (present + writable)
    page_directory[id] = ((uint32_t)last_page) | 3;

    // Advance last_page pointer to next free 4 KB page
    last_page = (uint32_t*)(((uint32_t)last_page) + 4096);
}

// Enable paging by loading CR3 and setting PG bit in CR0
static void paging_enable(void){

    // Load page directory base into CR3
    asm volatile("mov %0, %%cr3" : : "r"(page_dir_loc));
    
    // Set paging bit (bit 31) in CR0
    asm volatile(
        "mov %%cr0, %%eax\n\t"
        "orl $0x80000000, %%eax\n\t"
        "mov %%eax, %%cr0"
        :
        :
        : "%eax"
    );
}

// Initialize paging and identity-map first 8 MB
void init_paging(void){

    write_string("Setting up paging\n");

    // Place page directory and page tables at fixed addresses
    page_directory = (uint32_t*)0x400000;  // Page directory base
    page_dir_loc = (uint32_t)page_directory;
    last_page = (uint32_t*)0x404000;       // First page table base

    // Initialize directory: not present, writable, supervisor
    for (int i = 0; i < 1024; i++){
        page_directory[i] = 0x00000002;
    }

    // Identity-map 0x00000000–0x003FFFFF (first 4 MB)
    paging_map_virtual_to_phys(0x00000000, 0x00000000);

    // Identity-map 0x00400000–0x007FFFFF (next 4 MB)
    paging_map_virtual_to_phys(0x00400000, 0x00400000);

    // Enable paging
    paging_enable();

    write_string("Paging was successfully enabled!\n");
}
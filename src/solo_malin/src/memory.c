#include <libc/stdint.h>
#include "memory.h"
#include "screen.h"

static uint32_t* page_directory = 0;
static uint32_t page_dir_loc = 0;
static uint32_t* last_page = 0;

void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys){

    uint16_t id = virt >> 22;

    for (int i = 0; i < 1024; i++){
        last_page[i] = phys | 3;
        phys += 4096;
    }

    page_directory[id] = ((uint32_t)last_page) | 3;
    last_page = (uint32_t*)(((uint32_t)last_page) + 4096);
}

static void paging_enable(void){

    asm volatile("mov %0, %%cr3" : : "r"(page_dir_loc));
    asm volatile(
        "mov %%cr0, %%eax\n\t"
        "orl $0x80000000, %%eax\n\t"
        "mov %%eax, %%cr0"
        :
        :
        : "%eax"
    );
}

void init_paging(void){

    write_string("Setting up paging\n");

    page_directory = (uint32_t*)0x400000;
    page_dir_loc= (uint32_t)page_directory;
    last_page = (uint32_t*)0x404000;

    for (int i = 0; i < 1024; i++){
        page_directory[i] = 0x00000002;
    }

    paging_map_virtual_to_phys(0x00000000, 0x00000000);
    paging_map_virtual_to_phys(0x00400000, 0x00400000);

    paging_enable();

    write_string("Paging was successfully enabled!\n");
}
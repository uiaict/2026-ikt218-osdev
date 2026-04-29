#include "paging.h"
#include "screen.h"
#include "libc/stdint.h"

static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void init_paging(void) {
    uint32_t i;

    for (i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;
        first_page_table[i] = (i * 0x1000) | 3;
    }

    page_directory[0] = ((uint32_t)first_page_table) | 3;

    __asm__ volatile ("mov %0, %%cr3" :: "r"(page_directory));

    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile ("mov %0, %%cr0" :: "r"(cr0));

    screen_write("Paging initialized (first 4MB identity mapped)\n");
}
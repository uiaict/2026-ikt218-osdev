#include "paging.h"

typedef unsigned int uint32_t;

void terminal_write(const char* str);

extern void load_page_directory(uint32_t*);
extern void enable_paging(void);

static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void init_paging(void) {
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;
        first_page_table[i] = (i * 0x1000) | 3;
    }

    page_directory[0] = ((uint32_t)first_page_table) | 3;

    load_page_directory(page_directory);
    enable_paging();

    //terminal_write("Paging enabled\n"); // commented out to reduce clutter in terminal output
}
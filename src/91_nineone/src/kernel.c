#include "terminal.c"
#include "colors.h"
#include "libc/stdint.h"
#include "kernel/memory.h"

extern uint32_t end;

int main() {
    char *video_memory = (char*) 0xB8000; // startadresse for VGA tekstmodus

    const char *str = "Velkommen til DaviDOS!";
    
    print("Velkommen til FreDDaviDOS!", COLOR(YELLOW, BLUE), 0, 0);
    
    init_kernel_memory(&end);

    init_paging();

    print_memory_layout();

    void* some_memory = malloc(12345); 
    void* memory2 = malloc(54321); 
    void* memory3 = malloc(13331);

    while (1) { // coming soon
    }

    return 0;
}
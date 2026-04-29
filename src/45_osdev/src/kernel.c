#include <../include/libc/stdio.h>

// Simple Hello world function for testing puroses
void kernel_main(void) {
    printf("Helloo World!\n");
    
    // Infinite loop 
    while (1) {
        asm volatile("hlt");
    }
}

int main(void) {
    kernel_main();
    return 0;
}


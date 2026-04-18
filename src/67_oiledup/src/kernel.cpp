#include "gdt.h" // You need to include the header to use gdt_install()

extern "C" int kernel_main(void);

int kernel_main(){
    gdt_install();
    
    // An infinite loop at the end of the kernel is good practice
    // so it doesn't accidentally return into nothingness!
    while(1) {} 
    return 0;
}
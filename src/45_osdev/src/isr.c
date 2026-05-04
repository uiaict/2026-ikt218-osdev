#include <../include/libc/stdint.h>
#include "../include/libc/stdio.h"

void isr_common_c(int int_no, uint32_t err) {

    // This will print which interrupt fired
    printf("ISR %d triggered\n", int_no);
}

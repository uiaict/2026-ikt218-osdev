#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include <multiboot2.h>

struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

int kernel_main();

int compute (int a, int b) {
    return a + b;
}

int main(uint32_t magic, struct multiboot_info* mb_info_addr) {
    int noop = 0;
    int res = compute(1,2);

    return kernel_main();
}
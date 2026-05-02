#include "gdt.h"
#include "idt.h"
#include "matrix_rain.h"
#include "memory.h"
#include "pic.h"
#include "pit.h"
#include <libc/stdint.h>

extern uint32_t end;

void kernel_main(void)
{
    gdt_init();
    idt_init();
    pic_remap();
    pic_mask_all_except_timer_and_keyboard();

    init_kernel_memory(&end);
    init_paging();
    init_pit();

    asm volatile("sti");

    matrix_rain_demo();
}

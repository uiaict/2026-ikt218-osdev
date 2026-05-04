#include "terminal.h"
#include "menu.h"
#include "colors.h"
#include "libc/stdint.h"
#include "kernel/memory.h"
#include "libc/stdio.h"
#include "arch/i386/gdt.h"
#include "arch/i386/idt.h"
#include "arch/i386/isr.h"
#include "pit.h"
#include "keyboard.h"
#include "main_menu.h"
#include "apps/typegame/typegame.h"
#include "../include/apps/paint/paint.h"

extern uint32_t end;

int main(void) {
    init_gdt();

    terminal_clear(COLOR(WHITE, BLACK));
    printf_color(COLOR(YELLOW, BLUE), "Velkommen til FreDDaviDOS!");

    idt_init();

    init_kernel_memory(&end);
    init_paging();

    init_keyboard();
    init_pit();
    idt_enable_interrupts();

    init_menu();

    while (1) {
        sleep_interrupt(4); // 250 TPS
        switch (current_menu){
            case TYPEGAME_MENU:
                typegame_update();
                break;
            case PAINT_MENU:
                tick_brush();
                break;
            case MAIN_MENU:
                main_menu_update();
                break;
            default:
                break;
        }
    }
    return 0;
}
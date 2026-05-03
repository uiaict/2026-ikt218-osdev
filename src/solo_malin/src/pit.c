#include "pit.h"
#include "irq.h"
#include "isr.h"
#include "screen.h"
#include "util.h"
#include <libc/stdint.h>


static volatile uint32_t ticks = 0;

static void pit_callback(struct InterruptRegisters* r)
{
    (void)r;
    ticks++;
}

static uint32_t get_current_tick(void)
{
    return ticks;
}

void init_pit(){


    uint32_t divisor = DIVIDER;

    irq_install_handler(0, pit_callback);


    outPortB(PIT_CMD_PORT,0x36);  
    outPortB(PIT_CHANNEL0_PORT,(uint8_t)(divisor & 0xFF));
    outPortB(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));

    write_string("PIT initialized\n");
}

void sleep_busy(uint32_t millisecond){

    uint32_t start_tick = get_current_tick();
    uint32_t ticks_to_wait = millisecond * TICKS_PER_MS;
    uint32_t elapsed_ticks = 0;
    
    while (elapsed_ticks < ticks_to_wait){
        while (get_current_tick() == start_tick + elapsed_ticks) {};
        elapsed_ticks++;
    }
}

void sleep_interrupt(uint32_t millisecond){

    uint32_t current_tick = get_current_tick();
    uint32_t ticks_to_wait = millisecond * TICKS_PER_MS;
    uint32_t end_ticks = current_tick + ticks_to_wait;

    while (current_tick < end_ticks){
        __asm__ __volatile__("sti; hlt");
        current_tick = get_current_tick();
    }
}

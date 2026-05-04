#include <kernel/pit.h>
#include <io.h>
#include <irq.h>

void init_pit(void)
{
    // channel 0, lo/hi, mode 3 (square wave), binary
    outb(PIT_CMD_PORT, 0x36);

    uint32_t divisor = DIVIDER;
    outb(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));
}

static uint32_t get_current_tick(void)
{
    return timer_get_ticks();
}

void sleep_busy(uint32_t milliseconds)
{
    uint32_t start_tick    = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t elapsed_ticks = 0;

    while (elapsed_ticks < ticks_to_wait) {
        while (get_current_tick() == start_tick + elapsed_ticks)
            ;
        elapsed_ticks++;
    }
}

void sleep_interrupt(uint32_t milliseconds)
{
    uint32_t current_tick  = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t end_ticks     = current_tick + ticks_to_wait;

    while (current_tick < end_ticks) {
        __asm__ volatile ("sti\n\t"
                          "hlt\n\t");
        current_tick = get_current_tick();
    }
}

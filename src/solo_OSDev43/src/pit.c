#include "pit.h"

static volatile uint32_t timer_ticks = 0;

static inline void outb(unsigned short port, unsigned char value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void pit_tick(void) {
    timer_ticks++;
}

uint32_t get_current_tick(void) {
    return timer_ticks;
}

void init_pit(void) {
    unsigned int divisor = DIVIDER;

    outb(PIT_CMD_PORT, 0x36);
    outb(PIT_CHANNEL0_PORT, divisor & 0xFF);
    outb(PIT_CHANNEL0_PORT, (divisor >> 8) & 0xFF);
}

void sleep_busy(uint32_t milliseconds) {
    uint32_t start_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;

    while ((get_current_tick() - start_tick) < ticks_to_wait) {
    }
}

void sleep_interrupt(uint32_t milliseconds) {
    uint32_t start_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;

    while ((get_current_tick() - start_tick) < ticks_to_wait) {
        asm volatile("sti");
        asm volatile("hlt");
    }
}
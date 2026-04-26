#include "kernel/pit.h"
#include "libc/stdio.h"

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static volatile uint32_t ticks = 0;

void timer_interrupt_handler() {
    ticks++;
}

uint32_t get_current_tick() {
    return ticks;
}

void init_pit() {
    uint32_t divisor = DIVIDER;
    outb(PIT_CMD_PORT, 0x36);  // channel 0, lobyte/hibyte, square wave
    outb(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));
    printf("PIT initialized at %d Hz\n", TARGET_FREQUENCY);
}

void sleep_busy(uint32_t milliseconds) {
    uint32_t start_ticket = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t elapsed_ticks = 0;
    
    while (elapsed_ticks < ticks_to_wait) {
    while (get_current_tick() == start_ticket + elapsed_ticks) {
        //busy
    }
    elapsed_ticks++;
    }
}

void sleep_interrupt(uint32_t milliseconds) {
    uint32_t current_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t end_ticks = current_tick + ticks_to_wait;

    while (current_tick < end_ticks) {
        asm volatile ("sti");
        asm volatile ("hlt");
        current_tick = get_current_tick();
    }
}

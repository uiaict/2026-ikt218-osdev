#include "pit.h"

void init_pit() {
    uint16_t divisor = DIVIDER;

    // Command byte:
    // Channel 0, lobyte/hibyte, mode 3 (square wave), binary
    outb(PIT_CMD_PORT, 0x36);

    // Send divisor (low byte first, then high byte)
    outb(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));

}

void sleep_interrupt(uint32_t milliseconds) {
    uint32_t current_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t end_ticks = current_tick + ticks_to_wait;

    while (current_tick < end_ticks) {
        asm volatile("sti"); // enable interrupts
        asm volatile("hlt"); // sleep until next interrupt
        current_tick = get_current_tick();
    }
}

void sleep_busy(uint32_t milliseconds) {
    uint32_t start_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t elapsed_ticks = 0;

    while (elapsed_ticks < ticks_to_wait) {
        while (get_current_tick() == start_tick + elapsed_ticks) {
            // Busy wait
        }
        elapsed_ticks++;
    }
}

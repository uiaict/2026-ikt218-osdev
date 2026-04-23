#include "pit.h"
#include "ports.h"
#include "isr.h"

volatile uint32_t timer_ticks = 0;

void timer_callback(registers_t *r) {
    timer_ticks++;
}

void init_pit() {
    uint16_t divisor = DIVIDER;

    // Send the command byte (0x36 sets the PIT to square wave mode)
    port_byte_out(PIT_CMD_PORT, 0x36);

    // Send the divisor (split into low and high bytes)
    port_byte_out(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));
    port_byte_out(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));

    // Register our timer callback to IRQ0
    register_interrupt_handler(IRQ0, timer_callback);
}

void sleep_busy(uint32_t milliseconds) {
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t start_tick = timer_ticks;
    uint32_t elapsed_ticks = 0;
    
    while (elapsed_ticks < ticks_to_wait) {
        while (timer_ticks == start_tick + elapsed_ticks) {
            // Do nothing (busy wait)
        }
        elapsed_ticks++;
    }
}

void sleep_interrupt(uint32_t milliseconds) {
    uint32_t current_tick = timer_ticks;
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t end_ticks = current_tick + ticks_to_wait;
    
    while (current_tick < end_ticks) {
        __asm__ volatile("sti");
        __asm__ volatile("hlt");
        current_tick = timer_ticks;
    }
}
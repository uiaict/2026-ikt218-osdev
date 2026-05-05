#include "pit.h"
#include "ports.h"
#include "isr.h"

volatile uint32_t timer_ticks = 0;

void timer_callback(registers_t *r) {
    timer_ticks++;
}

void init_pit() {
    // Base frequency: 1193180 Hz. Target: 1000 Hz (1 ms per tick).
    // Divisor = 1193180 / 1000 = 1193
    uint16_t divisor = 1193;

    // Send the command byte (0x36 sets PIT to Channel 0, lobyte/hibyte, square wave mode)
    port_byte_out(0x43, 0x36);

    // Send the divisor to Channel 0 data port (0x40)
    port_byte_out(0x40, (uint8_t)(divisor & 0xFF));
    port_byte_out(0x40, (uint8_t)((divisor >> 8) & 0xFF));

    // Register our timer callback specifically to Interrupt 32 (IRQ0 mapped)
    register_interrupt_handler(32, timer_callback);
}

void sleep_busy(uint32_t milliseconds) {
    uint32_t ticks_to_wait = milliseconds; // 1 tick = 1 ms
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
    // Calculate the target tick to wake up
    uint32_t end_ticks = timer_ticks + milliseconds;
    
    while (timer_ticks < end_ticks) {
        __asm__ volatile("sti");
        __asm__ volatile("hlt");
    }
}
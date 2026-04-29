#include "pit.h"
#include <stdint.h>
#include "io.h"


volatile uint32_t tick = 0;

// IRQ0 handler
void pit_callback(struct regs* r) {
    (void)r;
    tick++;
}

// Initialiser PIT
void init_pit() {
    uint32_t divisor = DIVIDER;

    // Send command byte
    outb(PIT_CMD_PORT, 0x36);

    // Send divisor (low + high byte)

    outb(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));



}

// Busy wait sleep
void sleep_busy(uint32_t milliseconds) {
    uint32_t start = tick;
    uint32_t wait = milliseconds * TICKS_PER_MS;

    while ((tick - start) < wait) {
        // do nothing
    }
}

// Interrupt sleep
void sleep_interrupt(uint32_t milliseconds) {
    uint32_t end = tick + (milliseconds * TICKS_PER_MS);

    while (tick < end) {
        asm volatile("sti");
        asm volatile("hlt");
    }
}
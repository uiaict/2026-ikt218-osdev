#include "kernel/pit.h"
#include "io.h"
#include "libc/stdio.h"

extern uint32_t tick;  // shared with irq.c (incremented in IRQ0 handler)

uint32_t get_current_tick() {
    return tick;
}

void init_pit() {
    // PIT runs at 1193180 Hz; dividing it produces our target IRQ0 frequency.
    uint32_t divisor = DIVIDER;

    // 0x36 = channel 0, access mode lobyte/hibyte, mode 3 (square wave), binary.
    outb(PIT_CMD_PORT, 0x36);
    // The divisor is a 16-bit value, but the data port is 8 bits wide,
    // so we send the low byte first and then the high byte.
    outb(PIT_CHANNEL0_PORT, divisor & 0xFF);
    outb(PIT_CHANNEL0_PORT, (divisor >> 8) & 0xFF);

    printf("PIT initialized at %d Hz\n", TARGET_FREQUENCY);
}

// Spin-loop sleep: keeps the CPU fully busy while waiting.
// Wastes power, but useful when interrupts/halt are not safe to use.
void sleep_busy(uint32_t milliseconds) {
    uint32_t start_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t elapsed_ticks = 0;

    while (elapsed_ticks < ticks_to_wait) {
        asm volatile("sti");  // make sure interrupts are enabled so the tick counter can advance
        // Spin until the IRQ0 handler bumps `tick` past our last observed value.
        while (get_current_tick() == start_tick + elapsed_ticks) {
            // busy wait until the next PIT tick
        }
        elapsed_ticks++;
    }
}

// Interrupt-driven sleep: halts the CPU between ticks to save power.
// Each PIT interrupt wakes the CPU, then we re-check whether enough time has passed.
void sleep_interrupt(uint32_t milliseconds) {
    uint32_t current_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t end_ticks = current_tick + ticks_to_wait;

    while (current_tick < end_ticks) {
        // Enable interrupts and halt; the CPU resumes on the next interrupt (typically IRQ0).
        asm volatile("sti");
        asm volatile("hlt");
        current_tick = get_current_tick();
    }
}
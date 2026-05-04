#include "kernel/pit.h"
#include "../include/io.h"
#include <libc/stdio.h>
#include "../irq.h"
extern void music_tick();


// Tracks how many milliseconds have passed since PIT was initialized.
// volatile because it is modified inside an interrupt handler - without this
// the compiler might cache the value in a register and never see it update.
static volatile uint32_t tick = 0;

uint32_t get_current_tick() {
    return tick;
}

// Called automatically by the IRQ handler every millisecond.
// We just increment the tick counter, the sleep functions read this.
static void pit_handler() {
    tick++;
    music_tick();
}

void init_pit() {
    // Register our handler so irq_handler() calls pit_handler() when IRQ0 fires
    irq_install_handler(0, pit_handler);

    // Tell the PIT what frequency to fire at.
    // DIVIDER = 1193180 / 1000 = 1193, meaning the PIT counts down from
    // 1193 each time, giving us exactly 1000 interrupts per second (1 per ms)
    outb(PIT_CMD_PORT, 0x36);                        // channel 0, lobyte/hibyte, square wave mode
    outb(PIT_CHANNEL0_PORT, DIVIDER & 0xFF);         // send low byte of divisor
    outb(PIT_CHANNEL0_PORT, (DIVIDER >> 8) & 0xFF);  // send high byte of divisor

    printf("PIT initialized at %d Hz\n", TARGET_FREQUENCY);
}

void sleep_busy(uint32_t milliseconds) {
    uint32_t start_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t elapsed_ticks = 0;

    // Outer loop: keep going until enough ticks have passed
    while (elapsed_ticks < ticks_to_wait) {
        // Inner loop: wait here doing nothing until the tick counter advances.
        // This is the "busy" part - the CPU is fully occupied just checking a value.
        while (get_current_tick() == start_tick + elapsed_ticks);
        elapsed_ticks++;
    }
}

void sleep_interrupt(uint32_t milliseconds) {
    uint32_t current_tick = get_current_tick();
    uint32_t end_ticks = current_tick + (milliseconds * TICKS_PER_MS);

    while (current_tick < end_ticks) {
        // sti: enable interrupts so the PIT interrupt can actually fire
        // hlt: pause the CPU until the next interrupt arrives
        // Together these mean: sleep until something happens, then check if we are done.
        // This is efficient - the CPU does zero work while waiting.
        asm volatile("sti; hlt");
        current_tick = get_current_tick();
    }
}
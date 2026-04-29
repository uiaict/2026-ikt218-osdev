#include "pit.h"
#include "ports.h"
#include "irq.h"
#include "libc/stdint.h"

#define PIT_COMMAND_PORT   0x43
#define PIT_CHANNEL0_PORT  0x40

static volatile uint32_t pit_ticks = 0;
static uint32_t pit_frequency = 100; // 100 Hz = 10 ms per tick

static void pit_callback(struct registers* regs) {
    (void)regs;
    pit_ticks++;
}

void init_pit(void) {
    uint32_t divisor = PIT_BASE_FREQUENCY / pit_frequency;

    outb(PIT_COMMAND_PORT, 0x36);
    outb(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));

    irq_install_handler(0, pit_callback);
}

uint32_t pit_get_ticks(void) {
    return pit_ticks;
}

void pit_sleep_ms(uint32_t ms) {
    uint32_t ticks_to_wait = (ms * pit_frequency) / 1000;

    if (ticks_to_wait == 0) {
        ticks_to_wait = 1;
    }

    uint32_t start = pit_get_ticks();
    while ((pit_get_ticks() - start) < ticks_to_wait) {
        __asm__ volatile ("hlt");
    }
}
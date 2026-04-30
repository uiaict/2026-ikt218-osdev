#include "pit.h"
#include "common.h"
#include "interrupts.h"
#include "libc/stddef.h"
#include <libc/stdio.h>

static volatile uint32_t pit_ticks = 0;

static void pit_irq_handler(registers_t* regs, void* ctx)
{
    (void)regs;
    (void)ctx;
    pit_ticks++;
}

void init_pit(void)
{
    outb(PIT_CMD_PORT, 0x36);

    uint16_t divisor = (uint16_t)DIVIDER;
    outb(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));

    register_irq_handler(0, pit_irq_handler, NULL);

    printf("PIT initialized\n");
}

static inline uint32_t get_tick(void)
{
    return pit_ticks;
}

void sleep_busy(uint32_t milliseconds)
{
    uint32_t start = get_tick();
    while ((get_tick() - start) < milliseconds) {
    }
}

void sleep_interrupt(uint32_t milliseconds)
{
    uint32_t end = get_tick() + milliseconds;
    while (get_tick() < end) {
        asm volatile("sti; hlt");
    }
}

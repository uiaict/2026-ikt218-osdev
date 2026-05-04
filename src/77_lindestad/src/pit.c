#include <pit.h>

#include <interrupts.h>
#include <ports.h>

static volatile uint32_t pit_ticks;

static void pit_irq_handler(struct interrupt_registers* registers)
{
    (void)registers;
    pit_ticks++;
}

uint32_t pit_get_ticks(void)
{
    return pit_ticks;
}

void init_pit(void)
{
    pit_ticks = 0;

    interrupt_register_handler(32, pit_irq_handler);

    port_byte_out(PIT_CMD_PORT, 0x36);
    port_byte_out(PIT_CHANNEL0_PORT, (uint8_t)(PIT_DIVIDER & 0xFF));
    port_byte_out(PIT_CHANNEL0_PORT, (uint8_t)((PIT_DIVIDER >> 8) & 0xFF));
}

void sleep_busy(uint32_t milliseconds)
{
    uint32_t start = pit_get_ticks();
    uint32_t end = start + milliseconds;

    while (pit_get_ticks() < end) {
        __asm__ volatile("nop");
    }
}

void sleep_interrupt(uint32_t milliseconds)
{
    uint32_t start = pit_get_ticks();
    uint32_t end = start + milliseconds;

    while (pit_get_ticks() < end) {
        __asm__ volatile("sti");
        __asm__ volatile("hlt");
    }
}

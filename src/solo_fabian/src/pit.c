#include <pit.h>

/* Global timer tick counter, changed by IRQ0 */
static volatile uint32_t pit_ticks;

/* Write on byte to an x86 I/O port */
static void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

/* Configure PIT channel 0 to fire about once per millisecond */
void init_pit(void)
{
    pit_ticks = 0;

    outb(PIT_CMD_PORT, PIT_COMMAND);
    outb(PIT_CHANNEL0_PORT, (uint8_t)(PIT_DIVIDER & 0xFF));
    outb(PIT_CHANNEL0_PORT, (uint8_t)((PIT_DIVIDER >> 8) & 0xFF));
}   

/* Called from IRQ0 every time the pit fires */
void pit_handle_tick(void) 
{
    pit_ticks ++;   
}

/* Return the current timer tick count. */
uint32_t pit_get_ticks(void)
{
    return pit_ticks;
}

/* Sleep by spinning until enough PIT ticks have passed. */
void sleep_busy(uint32_t milliseconds)
{
    uint32_t start = pit_get_ticks();
    
    while (pit_get_ticks() - start < milliseconds) {}
}

/* SLeep by halting the CPU until timer interrupts wake it. */
void sleep_interrupt(uint32_t milliseconds) 
{
    uint32_t start = pit_get_ticks();

    while (pit_get_ticks() - start < milliseconds) {
        __asm__ volatile ("sti");
        __asm__ volatile ("hlt");
    }
}
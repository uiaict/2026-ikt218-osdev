#include "pit.h"
#include "arch/i386/isr.h"

// global tick counter, volatile since value changes
static volatile uint32_t pit_ticks = 0;

// write byte to I/O port.
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

// callback to handle pit ticks increment.
static void pit_callback(registers_t* regs) {
    (void)regs;
    pit_ticks++;
}

uint32_t get_current_tick(void) {
    return pit_ticks;
}

// 
void init_pit(void) {
    register_interrupt_handler(IRQ0, pit_callback);

    // calculate interrupt frequency.
    uint32_t divisor = PIT_BASE_FREQUENCY / PIT_TARGET_FREQUENCY;

    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

    /*
     * 0x36 means:
     * channel 0,
     * access mode: low byte then high byte,
     * mode 3 square wave generator,
     * binary mode.
     */
    outb(PIT_CMD_PORT, 0x36);
    outb(PIT_CHANNEL0_PORT, low);
    outb(PIT_CHANNEL0_PORT, high);
}

// polls tick counter until time has passed.
void sleep_busy(uint32_t milliseconds) {
    uint32_t start = get_current_tick();

    while (get_current_tick() - start < milliseconds) {
        // Busy wait
    }
}

// Like sleep_busy, but enables interrupts (sti) and halts CPU (hlt). Less load on CPU.
void sleep_interrupt(uint32_t milliseconds) {
    uint32_t start = get_current_tick();

    while (get_current_tick() - start < milliseconds) {
        __asm__ volatile ("sti; hlt");
    }
}
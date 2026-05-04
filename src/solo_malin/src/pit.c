#include "pit.h"
#include "irq.h"
#include "isr.h"
#include "screen.h"
#include "util.h"
#include <libc/stdint.h>

// ==============================
// Programmable Interval Timer (PIT)
//
// Handles timer ticks and provides
// simple sleep functions based on
// the PIT interrupt.
// ==============================

static volatile uint32_t ticks = 0;     // Global tick counter

// IRQ0 callback: runs on every PIT interrupt
static void pit_callback(struct InterruptRegisters* r)
{
    (void)r;   // Unused parameter
    ticks++;   // Count timer ticks
}

// Return current tick count
static uint32_t get_current_tick(void)
{
    return ticks;
}

// Initialize PIT to generate periodic interrupts
void init_pit(){


    uint32_t divisor = DIVIDER;     // Divisor for target frequency

    // Install PIT interrupt handler on IRQ0
    irq_install_handler(0, pit_callback);

    // Configure PIT channel 0:
    // 0x36 = channel 0, low/high byte, mode 3, binary
    outPortB(PIT_CMD_PORT,0x36);  
    outPortB(PIT_CHANNEL0_PORT,(uint8_t)(divisor & 0xFF));          // Low Byte
    outPortB(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));  // High byte

    write_string("PIT initialized\n");
}

// Sleep by actively waiting for enough ticks to pass
void sleep_busy(uint32_t millisecond){

    uint32_t start_tick = get_current_tick();
    uint32_t ticks_to_wait = millisecond * TICKS_PER_MS;
    uint32_t elapsed_ticks = 0;
    
    while (elapsed_ticks < ticks_to_wait){
        while (get_current_tick() == start_tick + elapsed_ticks) {};
        elapsed_ticks++;
    }
}

// Sleep by halting CPU until timer interrupts wake it up
void sleep_interrupt(uint32_t millisecond){

    uint32_t current_tick = get_current_tick();
    uint32_t ticks_to_wait = millisecond * TICKS_PER_MS;
    uint32_t end_ticks = current_tick + ticks_to_wait;

    while (current_tick < end_ticks){
        __asm__ __volatile__("sti; hlt");       // Enable interrupts and sleep
        current_tick = get_current_tick();
    }
}

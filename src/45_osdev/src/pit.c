#include "../include/pit.h"
#include <../include/libc/stdint.h>

// tick counter 
static volatile uint32_t pit_ticks = 0;

// Forward declarations for I/O functionality
extern uint8_t inb_port(uint16_t port);
extern void outb_port(uint16_t port, uint8_t val);

// Forward declaration for IRQ handler
extern void irq_handler_c(int irq);

// Get the current number of ticks since PIT initialization
// Each tick equals 1 millisecond
uint32_t get_current_tick(void)
{
    return pit_ticks;
}

// Internal IRQ0 handler that increments the tick counter
// it is called from irq_handler_c when irq == 0
void pit_irq_handler(void)
{
    pit_ticks++;
}

// Initialize the PIT to generate interrupts at TARGET_FREQUENCY (1000 Hz)
// This sets up Channel 0 in Mode 3 (Square Wave Generator)
void init_pit(void)
{
    // Channel 0, lobyte/hibyte, Mode 3, binary
    uint8_t cmd = 0x36;  
    outb_port(PIT_CMD_PORT, cmd);
    
    // Load the divisor in Channel 0
    // low byte first, then high byte
    uint16_t divisor = DIVIDER;  // 1193180 / 1000 = 1193
    
    // low byte
    outb_port(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));        
    // high byte
    outb_port(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF)); 
    
    pit_ticks = 0;  // Initialize tick counter
}

// Busy-wait sleep that is using the tick counter updated by the PIT IRQ handler. 
void sleep_busy(uint32_t milliseconds)
{
    uint32_t start_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t elapsed_ticks = 0;
    
    while (elapsed_ticks < ticks_to_wait) {
        // Busy wait until the next tick arrives
        while (get_current_tick() == start_tick + elapsed_ticks) {
            // Do nothing
        }
        elapsed_ticks++;
    }
}

// Sleep using interrupts: (CPU halt)
// This releases CPU time and is more efficient than busy waiting
// The CPU will be awakened by the next PIT interrupt

void sleep_interrupt(uint32_t milliseconds)
{
    uint32_t current_tick = get_current_tick();
    uint32_t ticks_to_wait = milliseconds * TICKS_PER_MS;
    uint32_t end_ticks = current_tick + ticks_to_wait;
    
    while (current_tick < end_ticks) {
        // Enable interrupts and halt CPU until next interrupt
      
        asm volatile("sti; hlt");
        
        // After interrupt, update current tick
        current_tick = get_current_tick();
    }
}

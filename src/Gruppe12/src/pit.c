#include "kernel/pit.h"
#include "terminal.h"

volatile uint32_t pit_ticks = 0;

void pit_tick_increment(void) {
    pit_ticks++;
}

void init_pit()
{
    uint8_t access_mode = 0x30;
    uint8_t operating_mode = 0x06;
    uint8_t bcd = 0x00;
    
    uint8_t control_byte = access_mode | operating_mode | bcd;
    
    asm volatile("outb %0, %1" : : "a"(control_byte), "Nd"(PIT_CMD_PORT));
    
    uint16_t divisor = DIVIDER;
    asm volatile("outb %0, %1" : : "a"((uint8_t)(divisor & 0xFF)), "Nd"(PIT_CHANNEL0_PORT));
    asm volatile("outb %0, %1" : : "a"((uint8_t)((divisor >> 8) & 0xFF)), "Nd"(PIT_CHANNEL0_PORT));
    
    terminal_write("[PIT] Initialized at 1000 Hz\n");
}

uint32_t get_current_tick()
{
    return pit_ticks;
}

void sleep_busy(uint32_t milliseconds)
{
    uint32_t start = pit_ticks;
    uint32_t target = start + milliseconds;
    
    while (pit_ticks < target) {
        asm volatile("" ::: "memory");
    }
}

void sleep_interrupt(uint32_t milliseconds)
{
    uint32_t target = pit_ticks + milliseconds;
    
    while (pit_ticks < target) {
        asm volatile("sti; hlt" ::: "memory");
    }
}

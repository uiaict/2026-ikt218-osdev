#ifndef PIT_H
#define PIT_H

#include <libc/stdint.h>

// ==============================
// Programmable Interval Timer (PIT)
//
// Provides timing functionality,
// including delays and periodic
// interrupts.
// ==============================

// PIT I/O ports
#define PIT_CMD_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40
#define PIT_CHANNEL1_PORT 0x41
#define PIT_CHANNEL2_PORT 0x42
#define PC_SPEAKER_PORT 0x61

// Default divisor (sets timer frequency)
#define PIT_DEFAULT_DIVISOR 0x4E20 // 20000, which gives about 18.2 Hz (1193180 / 20000)

// PIC (Programmable Interrupt Controller)
#define PIC1_CMD_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC_EOI 0x20            // End of interrupt signal

// PIT frequency settings
#define PIT_BASE_FREQUENCY 1193180
#define TARGET_FREQUENCY 1000 // 1000 Hz
#define DIVIDER (PIT_BASE_FREQUENCY / TARGET_FREQUENCY)
#define TICKS_PER_MS (TARGET_FREQUENCY / TARGET_FREQUENCY)

// Initialize the PIT
void init_pit();  

// Sleep using timer interrupts
void sleep_interrupt(uint32_t millisecond);

// Sleep using busy-wait loop
void sleep_busy(uint32_t millisecond);

#endif
#include "kernel/pcspk.h"

void enable_speaker() {
    uint8_t state = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, state | 0x03);  // Sett bit 0 og 1
}

void disable_speaker() {
    uint8_t state = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, state & ~0x03);  // Fjern bit 0 og 1
}

void play_sound(uint32_t frequency) {
    if (frequency == 0) return;
    
    uint32_t divisor = PIT_BASE_FREQ / frequency;
    
    // Konfigurer PIT Channel 2, mode 3 (square wave)
    outb(PIT_CMD_PORT, 0xB6);
    outb(PIT_CHANNEL2_PORT, divisor & 0xFF);
    outb(PIT_CHANNEL2_PORT, (divisor >> 8) & 0xFF);
    
    enable_speaker();
}

void stop_sound() {
    disable_speaker();
}
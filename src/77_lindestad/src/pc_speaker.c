#include <pc_speaker.h>

#include <pit.h>
#include <ports.h>

void enable_speaker(void)
{
    uint8_t state = port_byte_in(PC_SPEAKER_PORT);
    port_byte_out(PC_SPEAKER_PORT, state | 0x03);
}

void disable_speaker(void)
{
    uint8_t state = port_byte_in(PC_SPEAKER_PORT);
    port_byte_out(PC_SPEAKER_PORT, state & (uint8_t)~0x03);
}

void play_sound(uint32_t frequency)
{
    if (frequency == 0) {
        return;
    }

    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;
    if (divisor == 0) {
        divisor = 1;
    }

    port_byte_out(PIT_CMD_PORT, 0xB6);
    port_byte_out(PIT_CHANNEL2_PORT, (uint8_t)(divisor & 0xFF));
    port_byte_out(PIT_CHANNEL2_PORT, (uint8_t)((divisor >> 8) & 0xFF));
    enable_speaker();
}

void stop_sound(void)
{
    uint8_t state = port_byte_in(PC_SPEAKER_PORT);
    port_byte_out(PC_SPEAKER_PORT, state & (uint8_t)~0x02);
}

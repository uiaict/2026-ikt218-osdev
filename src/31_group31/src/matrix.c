#include "matrix.h"
#include "shell.h"
#include "pit.h"
#include <stdint.h>

extern volatile uint32_t timer_ticks;

// Kendi urettigimiz PRNG (Sözde Rastgele Sayı Üreticisi) - stdlib.h olmadigi icin mecburen yaziyoruz
static uint32_t rand_state = 1;
static uint32_t rand() {
    rand_state = rand_state * 1103515245 + 12345;
    return (uint32_t)(rand_state / 65536) % 32768;
}

static char random_char() {
    // ASCII 33 (!) ile 126 (~) arasi okunabilir semboller uretir
    return 33 + (rand() % 94);
}

void run_matrix() {
    // 1. Dogrudan VGA bellegine erisim (Direct Memory Mapping)
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    
    int drop_y[80];
    int drop_len[80];
    int drop_speed[80];
    int drop_timer[80];

    // 2. Hardware-Seeded RNG: Rastgeleligi donanim saatinden aliyoruz
    rand_state = timer_ticks;

    for (int i = 0; i < 80; i++) {
        drop_y[i] = -(rand() % 25);         // Ekranin disinda (yukarida) baslasin
        drop_len[i] = 5 + (rand() % 15);    // Kuyruk uzunlugu
        drop_speed[i] = 1 + (rand() % 3);   // Dusme hizi
        drop_timer[i] = 0;
    }

    // Ekrani tamamen siyaha boya
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = (uint16_t)' ' | 0x0200; // Siyah arkaplan, koyu yesil yazi (VGA Attributes)
    }

    matrix_running = 1;

    while (matrix_running) {
        for (int x = 0; x < 80; x++) {
            drop_timer[x]++;
            if (drop_timer[x] >= drop_speed[x]) {
                drop_timer[x] = 0;

                // En ustteki kuyrugu sil
                int tail_y = drop_y[x] - drop_len[x];
                if (tail_y >= 0 && tail_y < 25) vga[tail_y * 80 + x] = (uint16_t)' ' | 0x0000;

                drop_y[x]++; // Damlayi asagi kaydir

                // Damlayi ciz (VGA Color Attributes kullanimi)
                for (int i = 0; i < drop_len[x]; i++) {
                    int py = drop_y[x] - i;
                    if (py >= 0 && py < 25) {
                        uint8_t color;
                        if (i == 0) color = 0x0F; // Damlanin ucu: Parlak Beyaz
                        else if (i < 3) color = 0x0A; // Damlanin govdesi: Acik Yesil
                        else color = 0x02; // Damlanin kuyrugu: Koyu Yesil

                        if (i == 0) vga[py * 80 + x] = (uint16_t)random_char() | (color << 8);
                        else {
                            uint16_t existing_char = vga[py * 80 + x] & 0x00FF;
                            if ((rand() % 10) == 0) existing_char = random_char(); // Glitch animasyonu
                            vga[py * 80 + x] = existing_char | (color << 8);
                        }
                    }
                }
                if (drop_y[x] - drop_len[x] >= 25) { // Ekrandan cikan damlayi yukari al
                    drop_y[x] = -(rand() % 10);
                    drop_len[x] = 5 + (rand() % 15);
                    drop_speed[x] = 1 + (rand() % 3);
                }
            }
        }
        sleep_busy(30); // FPS Ayari
    }
}
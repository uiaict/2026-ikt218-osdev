#include "matrix.h"
#include "shell.h"
#include "pit.h"
#include "stdint.h"

extern volatile uint32_t timer_ticks;

#define MATRIX_COLS 80
#define MATRIX_ROWS 24
#define MATRIX_STATUS_ROW 24
#define MATRIX_COLOR_MODES 4
#define MATRIX_MIN_DELAY 10
#define MATRIX_MAX_DELAY 80
#define MATRIX_DELAY_STEP 5

static volatile int matrix_color_mode = 0;
static volatile int matrix_frame_delay = 30;
static volatile int matrix_paused = 0;
static volatile int matrix_reset_requested = 0;

static uint32_t rand_state = 1;
static uint32_t rand() {
    rand_state = rand_state * 1103515245 + 12345;
    return (uint32_t)(rand_state / 65536) % 32768;
}

static char random_char() {
    return 33 + (rand() % 94);
}

static void vga_put(volatile uint16_t* vga, int row, int col, char c, uint8_t attr) {
    if (row < 0 || row >= 25 || col < 0 || col >= MATRIX_COLS) return;
    vga[row * MATRIX_COLS + col] = (uint16_t)c | ((uint16_t)attr << 8);
}

static void vga_write(volatile uint16_t* vga, int row, int col, const char* text, uint8_t attr) {
    int i = 0;
    while (text[i] && col + i < MATRIX_COLS) {
        vga_put(vga, row, col + i, text[i], attr);
        i++;
    }
}

static void clear_row(volatile uint16_t* vga, int row, uint8_t attr) {
    for (int x = 0; x < MATRIX_COLS; x++) {
        vga_put(vga, row, x, ' ', attr);
    }
}

static void clear_matrix_area(volatile uint16_t* vga) {
    for (int i = 0; i < MATRIX_COLS * MATRIX_ROWS; i++) {
        vga[i] = (uint16_t)' ' | 0x0200;
    }
}

static uint8_t color_for_depth(int depth) {
    int mode = matrix_color_mode % MATRIX_COLOR_MODES;

    if (depth == 0) return 0x0F;

    if (mode == 1) {
        if (depth < 3) return 0x0B;
        return 0x03;
    }
    if (mode == 2) {
        if (depth < 3) return 0x0E;
        return 0x06;
    }
    if (mode == 3) {
        if (depth < 3) return 0x0D;
        return 0x05;
    }

    if (depth < 3) return 0x0A;
    return 0x02;
}

static const char* color_mode_name() {
    int mode = matrix_color_mode % MATRIX_COLOR_MODES;
    if (mode == 1) return "cyan";
    if (mode == 2) return "amber";
    if (mode == 3) return "violet";
    return "green";
}

static const char* speed_name() {
    if (matrix_frame_delay <= 20) return "fast";
    if (matrix_frame_delay >= 45) return "slow";
    return "normal";
}

static void draw_status(volatile uint16_t* vga) {
    clear_row(vga, MATRIX_STATUS_ROW, 0x00);
    vga_write(vga, MATRIX_STATUS_ROW, 0, "ESC exit  c color  f/s speed  p pause  r reset", 0x0F);
    vga_write(vga, MATRIX_STATUS_ROW, 50, "mode:", 0x08);
    vga_write(vga, MATRIX_STATUS_ROW, 56, color_mode_name(), 0x0F);
    vga_write(vga, MATRIX_STATUS_ROW, 64, "speed:", 0x08);
    vga_write(vga, MATRIX_STATUS_ROW, 71, speed_name(), 0x0F);
    if (matrix_paused) {
        vga_write(vga, MATRIX_STATUS_ROW, 40, "PAUSED", 0x0E);
    }
}

static void reset_drops(int drop_y[], int drop_len[], int drop_speed[], int drop_timer[]) {
    for (int i = 0; i < MATRIX_COLS; i++) {
        drop_y[i] = -(rand() % MATRIX_ROWS);
        drop_len[i] = 5 + (rand() % 15);
        drop_speed[i] = 1 + (rand() % 3);
        drop_timer[i] = 0;
    }
}

void matrix_handle_key(char c) {
    if (c == 'q' || c == 'Q') {
        matrix_running = 0;
    } else if (c == 'c' || c == 'C') {
        matrix_color_mode = (matrix_color_mode + 1) % MATRIX_COLOR_MODES;
    } else if (c == 'f' || c == 'F') {
        if (matrix_frame_delay > MATRIX_MIN_DELAY) matrix_frame_delay -= MATRIX_DELAY_STEP;
    } else if (c == 's' || c == 'S') {
        if (matrix_frame_delay < MATRIX_MAX_DELAY) matrix_frame_delay += MATRIX_DELAY_STEP;
    } else if (c == 'p' || c == 'P') {
        matrix_paused = !matrix_paused;
    } else if (c == 'r' || c == 'R') {
        matrix_reset_requested = 1;
    }
}

void run_matrix() {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;

    int drop_y[MATRIX_COLS];
    int drop_len[MATRIX_COLS];
    int drop_speed[MATRIX_COLS];
    int drop_timer[MATRIX_COLS];

    rand_state = timer_ticks;
    reset_drops(drop_y, drop_len, drop_speed, drop_timer);
    matrix_paused = 0;
    matrix_reset_requested = 0;

    clear_matrix_area(vga);
    draw_status(vga);

    matrix_running = 1;

    while (matrix_running) {
        draw_status(vga);

        if (matrix_paused) {
            sleep_busy(60);
            continue;
        }

        if (matrix_reset_requested) {
            rand_state ^= timer_ticks;
            reset_drops(drop_y, drop_len, drop_speed, drop_timer);
            clear_matrix_area(vga);
            matrix_reset_requested = 0;
        }

        for (int x = 0; x < MATRIX_COLS; x++) {
            drop_timer[x]++;
            if (drop_timer[x] >= drop_speed[x]) {
                drop_timer[x] = 0;

                int tail_y = drop_y[x] - drop_len[x];
                if (tail_y >= 0 && tail_y < MATRIX_ROWS) vga[tail_y * MATRIX_COLS + x] = (uint16_t)' ' | 0x0000;

                drop_y[x]++;

                for (int i = 0; i < drop_len[x]; i++) {
                    int py = drop_y[x] - i;
                    if (py >= 0 && py < MATRIX_ROWS) {
                        uint8_t color = color_for_depth(i);

                        if (i == 0) vga[py * MATRIX_COLS + x] = (uint16_t)random_char() | ((uint16_t)color << 8);
                        else {
                            uint16_t existing_char = vga[py * MATRIX_COLS + x] & 0x00FF;
                            if ((rand() % 10) == 0) existing_char = random_char();
                            vga[py * MATRIX_COLS + x] = existing_char | ((uint16_t)color << 8);
                        }
                    }
                }
                if (drop_y[x] - drop_len[x] >= MATRIX_ROWS) {
                    drop_y[x] = -(rand() % 10);
                    drop_len[x] = 5 + (rand() % 15);
                    drop_speed[x] = 1 + (rand() % 3);
                }
            }
        }
        sleep_busy(matrix_frame_delay);
    }
}

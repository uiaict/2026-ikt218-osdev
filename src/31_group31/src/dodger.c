#include "dodger.h"
#include "pit.h"
#include "shell.h"
#include "song.h"
#include "stdint.h"

extern volatile uint32_t timer_ticks;

#define DODGER_COLS 80
#define DODGER_ROWS 25
#define DODGER_PLAY_ROWS 23
#define DODGER_MESSAGE_ROW 23
#define DODGER_STATUS_ROW 24
#define DODGER_PLAYER_ROW 22
#define DODGER_MAX_DROPS 30
#define DODGER_MAX_BOSS_SHOTS 34
#define DODGER_INPUT_STEP 8
#define DODGER_MOVE_REPEAT_MS 48
#define DODGER_MIN_DELAY 30
#define DODGER_START_DELAY 92
#define DODGER_BOSS_DELAY 62
#define DODGER_ELITE_FIRST_SCORE 360
#define DODGER_ELITE_INTERVAL 420
#define DODGER_ELITE_WARNING_FRAMES 18
#define DODGER_BOSS_WIDTH 11
#define DODGER_BOSS_WAVES 10
#define DODGER_BOSS_LEVEL_INTERVAL 20
#define DODGER_BOSS_WARNING_FRAMES 42
#define DODGER_BOSS_ATTACK_DELAY 18

#define DODGER_SCANCODE_ESC 0x01
#define DODGER_SCANCODE_Q 0x10
#define DODGER_SCANCODE_A 0x1E
#define DODGER_SCANCODE_D 0x20
#define DODGER_SCANCODE_P 0x19
#define DODGER_SCANCODE_M 0x32
#define DODGER_SCANCODE_RELEASE 0x80

typedef struct {
    int x;
    int y;
    int active;
    char glyph;
} DodgerDrop;

typedef struct {
    int x;
    int y;
    int width;
    int dir;
    int warning;
    int active;
} DodgerElite;

typedef struct {
    int x;
    int y;
    int dx;
    int active;
    char glyph;
} BossShot;

typedef struct {
    int x;
    int dir;
    int hp;
    int pattern;
    int attack_timer;
    int active;
} DodgerBoss;

static volatile int dodger_move_request = 0;
static volatile int dodger_left_down = 0;
static volatile int dodger_right_down = 0;
static volatile int dodger_last_direction = 0;
static volatile int dodger_p_down = 0;
static volatile int dodger_m_down = 0;
static volatile int dodger_paused = 0;
static volatile int dodger_music_enabled = 1;
static uint32_t dodger_rand_state = 1;
static uint32_t dodger_music_index = 0;
static uint32_t dodger_music_timer = 0;
static uint32_t dodger_move_cooldown = 0;

static uint32_t dodger_rand() {
    dodger_rand_state = dodger_rand_state * 1103515245 + 12345;
    return (uint32_t)(dodger_rand_state / 65536) % 32768;
}

static char dodger_random_char() {
    return 33 + (dodger_rand() % 94);
}

static void vga_put(volatile uint16_t* vga, int row, int col, char c, uint8_t attr) {
    if (row < 0 || row >= DODGER_ROWS || col < 0 || col >= DODGER_COLS) return;
    vga[row * DODGER_COLS + col] = (uint16_t)c | ((uint16_t)attr << 8);
}

static void vga_write(volatile uint16_t* vga, int row, int col, const char* text, uint8_t attr) {
    int i = 0;
    while (text[i] && col + i < DODGER_COLS) {
        vga_put(vga, row, col + i, text[i], attr);
        i++;
    }
}

static void clear_screen(volatile uint16_t* vga) {
    for (int i = 0; i < DODGER_COLS * DODGER_ROWS; i++) {
        vga[i] = (uint16_t)' ' | 0x0000;
    }
}

static void clear_playfield(volatile uint16_t* vga) {
    for (int y = 0; y < DODGER_PLAY_ROWS; y++) {
        for (int x = 0; x < DODGER_COLS; x++) {
            vga_put(vga, y, x, ' ', 0x00);
        }
    }
}

static void clear_row(volatile uint16_t* vga, int row) {
    for (int x = 0; x < DODGER_COLS; x++) vga_put(vga, row, x, ' ', 0x00);
}

static void write_number(volatile uint16_t* vga, int row, int col, uint32_t value, uint8_t attr) {
    char digits[12];
    int pos = 0;

    if (value == 0) {
        vga_put(vga, row, col, '0', attr);
        return;
    }

    while (value > 0 && pos < 11) {
        digits[pos++] = '0' + (value % 10);
        value /= 10;
    }

    for (int i = pos - 1; i >= 0; i--) {
        vga_put(vga, row, col++, digits[i], attr);
    }
}

static void draw_status(volatile uint16_t* vga, uint32_t score, uint32_t level, DodgerBoss* boss) {
    clear_row(vga, DODGER_STATUS_ROW);

    vga_write(vga, DODGER_STATUS_ROW, 0, "a/d move  p pause  m music  ESC exit", 0x0F);
    vga_write(vga, DODGER_STATUS_ROW, 38, "score:", 0x08);
    write_number(vga, DODGER_STATUS_ROW, 45, score, 0x0F);
    vga_write(vga, DODGER_STATUS_ROW, 55, "level:", 0x08);
    write_number(vga, DODGER_STATUS_ROW, 62, level, 0x0F);

    if (boss->active) {
        vga_write(vga, DODGER_STATUS_ROW, 68, "boss:", 0x0C);
        write_number(vga, DODGER_STATUS_ROW, 74, boss->hp, 0x0F);
    } else if (dodger_music_enabled) {
        vga_write(vga, DODGER_STATUS_ROW, 68, "music:on", 0x0A);
    } else {
        vga_write(vga, DODGER_STATUS_ROW, 68, "music:off", 0x08);
    }
}

static void draw_center_message(volatile uint16_t* vga, const char* text, uint8_t attr) {
    int len = 0;
    while (text[len]) len++;
    clear_row(vga, DODGER_MESSAGE_ROW);
    vga_write(vga, DODGER_MESSAGE_ROW, (DODGER_COLS - len) / 2, text, attr);
}

static void spawn_one_drop(DodgerDrop drops[]) {
    for (int i = 0; i < DODGER_MAX_DROPS; i++) {
        if (drops[i].active) continue;

        drops[i].active = 1;
        drops[i].x = dodger_rand() % DODGER_COLS;
        drops[i].y = 0;
        drops[i].glyph = dodger_random_char();
        return;
    }
}

static void spawn_drops(DodgerDrop drops[], uint32_t level) {
    uint32_t chance = 24;
    uint32_t attempts = 1 + (level / 6);

    if (level < 16) chance -= level / 2;
    else chance = 12;
    if (chance < 8) chance = 8;
    if (attempts > 3) attempts = 3;

    for (uint32_t i = 0; i < attempts; i++) {
        if ((dodger_rand() % chance) == 0) spawn_one_drop(drops);
    }
}

static void start_elite(DodgerElite* elite, uint32_t level) {
    elite->width = 5 + ((level / 5) % 2) * 2;
    elite->x = dodger_rand() % (DODGER_COLS - elite->width);
    elite->y = 0;
    elite->dir = (dodger_rand() % 2) ? 1 : -1;
    elite->warning = DODGER_ELITE_WARNING_FRAMES;
    elite->active = 0;
}

static void update_elite(DodgerElite* elite) {
    if (elite->warning > 0) {
        elite->warning--;
        if (elite->warning == 0) {
            elite->active = 1;
            elite->y = 0;
        }
        return;
    }

    if (!elite->active) return;

    elite->y++;
    if ((elite->y % 2) == 0) elite->x += elite->dir;

    if (elite->x <= 0) {
        elite->x = 0;
        elite->dir = 1;
    }
    if (elite->x + elite->width >= DODGER_COLS) {
        elite->x = DODGER_COLS - elite->width - 1;
        elite->dir = -1;
    }
    if (elite->y >= DODGER_PLAY_ROWS) elite->active = 0;
}

static int elite_hits_player(DodgerElite* elite, int player_x) {
    int player_left = player_x - 1;
    int player_right = player_x + 1;
    int enemy_left = elite->x;
    int enemy_right = elite->x + elite->width - 1;

    if (!elite->active || elite->y != DODGER_PLAYER_ROW) return 0;
    return player_right >= enemy_left && player_left <= enemy_right;
}

static void draw_elite(volatile uint16_t* vga, DodgerElite* elite) {
    if (elite->warning > 0) {
        vga_put(vga, 0, elite->x + (elite->width / 2), '!', 0x0E);
        return;
    }

    if (!elite->active) return;

    for (int i = 0; i < elite->width; i++) {
        char glyph = '#';
        if (i == 0) glyph = '<';
        else if (i == elite->width - 1) glyph = '>';
        vga_put(vga, elite->y, elite->x + i, glyph, 0x0C);
    }
}

static void start_real_boss(DodgerBoss* boss, uint32_t level) {
    boss->x = (DODGER_COLS - DODGER_BOSS_WIDTH) / 2;
    boss->dir = 1;
    boss->hp = DODGER_BOSS_WAVES + (level / 20);
    boss->pattern = 0;
    boss->attack_timer = DODGER_BOSS_ATTACK_DELAY;
    boss->active = 1;
}

static void spawn_boss_shot(BossShot shots[], int x, int dx, char glyph) {
    if (x < 0 || x >= DODGER_COLS) return;

    for (int i = 0; i < DODGER_MAX_BOSS_SHOTS; i++) {
        if (shots[i].active) continue;

        shots[i].active = 1;
        shots[i].x = x;
        shots[i].y = 4;
        shots[i].dx = dx;
        shots[i].glyph = glyph;
        return;
    }
}

static void launch_boss_pattern(DodgerBoss* boss, BossShot shots[], int player_x) {
    int center = boss->x + (DODGER_BOSS_WIDTH / 2);
    int pattern = boss->pattern % 4;

    if (pattern == 0) {
        int offsets[] = {-8, -4, 0, 4, 8};
        for (int i = 0; i < 5; i++) spawn_boss_shot(shots, center + offsets[i], 0, '|');
    } else if (pattern == 1) {
        spawn_boss_shot(shots, center, -1, '\\');
        spawn_boss_shot(shots, center, 0, '|');
        spawn_boss_shot(shots, center, 1, '/');
        spawn_boss_shot(shots, center - 5, 0, '|');
        spawn_boss_shot(shots, center + 5, 0, '|');
    } else if (pattern == 2) {
        spawn_boss_shot(shots, player_x, 0, '!');
        spawn_boss_shot(shots, player_x - 3, 0, '!');
        spawn_boss_shot(shots, player_x + 3, 0, '!');
    } else {
        int gap = player_x;
        for (int x = 2; x < DODGER_COLS - 2; x += 6) {
            if (x > gap - 5 && x < gap + 5) continue;
            spawn_boss_shot(shots, x, 0, '|');
        }
    }

    boss->pattern++;
    boss->hp--;
    boss->attack_timer = DODGER_BOSS_ATTACK_DELAY;
}

static void update_real_boss(DodgerBoss* boss, BossShot shots[], int player_x, int* clear_message_timer) {
    if (!boss->active) return;

    boss->x += boss->dir;
    if (boss->x <= 1) {
        boss->x = 1;
        boss->dir = 1;
    }
    if (boss->x + DODGER_BOSS_WIDTH >= DODGER_COLS - 1) {
        boss->x = DODGER_COLS - DODGER_BOSS_WIDTH - 1;
        boss->dir = -1;
    }

    boss->attack_timer--;
    if (boss->attack_timer <= 0) {
        launch_boss_pattern(boss, shots, player_x);
        if (boss->hp <= 0) {
            boss->active = 0;
            *clear_message_timer = 55;
        }
    }
}

static void draw_real_boss(volatile uint16_t* vga, DodgerBoss* boss) {
    if (!boss->active) return;

    vga_write(vga, 1, boss->x, "  /###\\  ", 0x04);
    vga_write(vga, 2, boss->x, " <|#-#|> ", 0x0C);
    vga_write(vga, 3, boss->x, "  \\_V_/  ", 0x04);
}

static int update_boss_shots(BossShot shots[], int player_x) {
    int player_left = player_x - 1;
    int player_right = player_x + 1;

    for (int i = 0; i < DODGER_MAX_BOSS_SHOTS; i++) {
        if (!shots[i].active) continue;

        shots[i].y++;
        if ((shots[i].y % 2) == 0) shots[i].x += shots[i].dx;

        if (shots[i].y == DODGER_PLAYER_ROW && shots[i].x >= player_left && shots[i].x <= player_right) {
            return 1;
        }

        if (shots[i].y >= DODGER_PLAY_ROWS || shots[i].x < 0 || shots[i].x >= DODGER_COLS) {
            shots[i].active = 0;
        }
    }

    return 0;
}

static void draw_boss_shots(volatile uint16_t* vga, BossShot shots[]) {
    for (int i = 0; i < DODGER_MAX_BOSS_SHOTS; i++) {
        if (!shots[i].active) continue;
        vga_put(vga, shots[i].y, shots[i].x, shots[i].glyph, 0x0E);
    }
}

static void update_music() {
    static uint32_t notes[] = {E4, R, G4, R, B4, R, E5, R, D5, R, B4, R, G4, R};
    static uint32_t durations[] = {4, 1, 4, 1, 4, 1, 6, 2, 4, 1, 4, 1, 6, 2};
    uint32_t n_notes = sizeof(notes) / sizeof(uint32_t);

    if (!dodger_music_enabled) {
        stop_sound();
        return;
    }

    if (dodger_music_timer > 0) {
        dodger_music_timer--;
        return;
    }

    if (notes[dodger_music_index] == R) stop_sound();
    else play_sound(notes[dodger_music_index]);

    dodger_music_timer = durations[dodger_music_index];
    dodger_music_index++;
    if (dodger_music_index >= n_notes) dodger_music_index = 0;
}

static void draw_drop(volatile uint16_t* vga, DodgerDrop* drop) {
    uint8_t color = 0x02;
    if (drop->y > DODGER_PLAYER_ROW - 6) color = 0x0A;
    if (drop->y > DODGER_PLAYER_ROW - 2) color = 0x0F;
    vga_put(vga, drop->y, drop->x, drop->glyph, color);
}

static void erase_player(volatile uint16_t* vga, int player_x) {
    vga_put(vga, DODGER_PLAYER_ROW, player_x - 1, ' ', 0x00);
    vga_put(vga, DODGER_PLAYER_ROW, player_x, ' ', 0x00);
    vga_put(vga, DODGER_PLAYER_ROW, player_x + 1, ' ', 0x00);
}

static void draw_player(volatile uint16_t* vga, int player_x) {
    vga_put(vga, DODGER_PLAYER_ROW, player_x - 1, '/', 0x0B);
    vga_put(vga, DODGER_PLAYER_ROW, player_x, '@', 0x0F);
    vga_put(vga, DODGER_PLAYER_ROW, player_x + 1, '\\', 0x0B);
}

static int current_move_direction() {
    if (dodger_move_request != 0) {
        int move = dodger_move_request;
        dodger_move_request = 0;
        dodger_move_cooldown = DODGER_MOVE_REPEAT_MS;
        return move;
    }

    if (dodger_move_cooldown > 0) return 0;

    if (dodger_left_down && dodger_right_down) {
        dodger_move_cooldown = DODGER_MOVE_REPEAT_MS;
        return dodger_last_direction;
    }
    if (dodger_left_down) {
        dodger_move_cooldown = DODGER_MOVE_REPEAT_MS;
        return -1;
    }
    if (dodger_right_down) {
        dodger_move_cooldown = DODGER_MOVE_REPEAT_MS;
        return 1;
    }

    return 0;
}

static void update_move_cooldown(uint32_t milliseconds) {
    if (dodger_move_cooldown <= milliseconds) {
        dodger_move_cooldown = 0;
    } else {
        dodger_move_cooldown -= milliseconds;
    }
}

static void apply_player_input(volatile uint16_t* vga, int* player_x) {
    int move = current_move_direction();

    if (move == 0) return;

    erase_player(vga, *player_x);
    if (move < 0 && *player_x > 1) (*player_x)--;
    if (move > 0 && *player_x < DODGER_COLS - 2) (*player_x)++;
    draw_player(vga, *player_x);
}

static void responsive_wait(volatile uint16_t* vga, int* player_x, uint32_t milliseconds) {
    uint32_t waited = 0;

    while (waited < milliseconds && dodger_running && !dodger_paused) {
        apply_player_input(vga, player_x);
        sleep_busy(DODGER_INPUT_STEP);
        update_move_cooldown(DODGER_INPUT_STEP);
        waited += DODGER_INPUT_STEP;
    }
}

static void play_game_over_sound() {
    uint32_t tones[] = {C4, G3, C3};
    for (uint32_t i = 0; i < 3; i++) {
        play_sound(tones[i]);
        sleep_interrupt(120);
        stop_sound();
        sleep_interrupt(60);
    }
}

void dodger_handle_key(char c) {
    if (c == 'q' || c == 'Q') {
        dodger_running = 0;
    } else if (c == 'a' || c == 'A') {
        dodger_move_request = -1;
        dodger_last_direction = -1;
    } else if (c == 'd' || c == 'D') {
        dodger_move_request = 1;
        dodger_last_direction = 1;
    } else if (c == 'p' || c == 'P') {
        dodger_paused = !dodger_paused;
    } else if (c == 'm' || c == 'M') {
        dodger_music_enabled = !dodger_music_enabled;
    }
}

void dodger_handle_scancode(uint8_t scancode) {
    uint8_t code = scancode & 0x7F;
    int released = (scancode & DODGER_SCANCODE_RELEASE) != 0;

    if (code == DODGER_SCANCODE_A) {
        int was_down = dodger_left_down;
        dodger_left_down = !released;
        if (!released) {
            dodger_last_direction = -1;
            if (!was_down) dodger_move_request = -1;
        } else if (dodger_last_direction == -1) {
            dodger_last_direction = dodger_right_down ? 1 : 0;
        }
    } else if (code == DODGER_SCANCODE_D) {
        int was_down = dodger_right_down;
        dodger_right_down = !released;
        if (!released) {
            dodger_last_direction = 1;
            if (!was_down) dodger_move_request = 1;
        } else if (dodger_last_direction == 1) {
            dodger_last_direction = dodger_left_down ? -1 : 0;
        }
    } else if ((code == DODGER_SCANCODE_ESC || code == DODGER_SCANCODE_Q) && !released) {
        dodger_running = 0;
    } else if (code == DODGER_SCANCODE_P) {
        if (released) {
            dodger_p_down = 0;
        } else if (!dodger_p_down) {
            dodger_paused = !dodger_paused;
            dodger_p_down = 1;
        }
    } else if (code == DODGER_SCANCODE_M) {
        if (released) {
            dodger_m_down = 0;
        } else if (!dodger_m_down) {
            dodger_music_enabled = !dodger_music_enabled;
            dodger_m_down = 1;
        }
    }
}

void run_dodger() {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    DodgerDrop drops[DODGER_MAX_DROPS];
    BossShot boss_shots[DODGER_MAX_BOSS_SHOTS];
    DodgerElite elite;
    DodgerBoss boss;
    int player_x = DODGER_COLS / 2;
    uint32_t score = 0;
    uint32_t level = 1;
    uint32_t frame_delay = DODGER_START_DELAY;
    uint32_t next_elite_score = DODGER_ELITE_FIRST_SCORE;
    uint32_t next_boss_level = DODGER_BOSS_LEVEL_INTERVAL;
    int boss_warning = 0;
    int boss_clear_message = 0;
    int game_over = 0;

    dodger_rand_state = timer_ticks ^ 0xD06E31;
    dodger_running = 1;
    dodger_move_request = 0;
    dodger_left_down = 0;
    dodger_right_down = 0;
    dodger_last_direction = 0;
    dodger_p_down = 0;
    dodger_m_down = 0;
    dodger_move_cooldown = 0;
    dodger_paused = 0;
    dodger_music_enabled = 1;
    dodger_music_index = 0;
    dodger_music_timer = 0;
    elite.active = 0;
    elite.warning = 0;
    boss.active = 0;

    for (int i = 0; i < DODGER_MAX_DROPS; i++) drops[i].active = 0;
    for (int i = 0; i < DODGER_MAX_BOSS_SHOTS; i++) boss_shots[i].active = 0;

    clear_screen(vga);

    while (dodger_running && !game_over) {
        if (dodger_paused) {
            draw_status(vga, score, level, &boss);
            draw_center_message(vga, "PAUSED", 0x0E);
            stop_sound();
            sleep_busy(80);
            continue;
        }

        update_music();

        apply_player_input(vga, &player_x);

        level = 1 + (score / 120);
        frame_delay = DODGER_START_DELAY;
        if (level < 14) frame_delay -= level * 4;
        else frame_delay = DODGER_MIN_DELAY;
        if (frame_delay < DODGER_MIN_DELAY) frame_delay = DODGER_MIN_DELAY;

        if (!boss.active && boss_warning == 0 && level >= next_boss_level) {
            boss_warning = DODGER_BOSS_WARNING_FRAMES;
            next_boss_level += DODGER_BOSS_LEVEL_INTERVAL;
        }

        if (boss_warning > 0) {
            boss_warning--;
            if (boss_warning == 0) {
                start_real_boss(&boss, level);
                elite.active = 0;
                elite.warning = 0;
                for (int i = 0; i < DODGER_MAX_DROPS; i++) drops[i].active = 0;
            }
        }

        if (boss.active || boss_warning > 0) frame_delay = DODGER_BOSS_DELAY;

        if (!boss.active && boss_warning == 0 && !elite.active && elite.warning == 0 && score >= next_elite_score) {
            start_elite(&elite, level);
            next_elite_score = score + DODGER_ELITE_INTERVAL + (dodger_rand() % 80);
        }

        if (!boss.active) {
            spawn_drops(drops, level);
            update_elite(&elite);
        }

        update_real_boss(&boss, boss_shots, player_x, &boss_clear_message);
        if (update_boss_shots(boss_shots, player_x)) game_over = 1;

        clear_playfield(vga);
        clear_row(vga, DODGER_MESSAGE_ROW);

        for (int i = 0; i < DODGER_MAX_DROPS; i++) {
            if (!drops[i].active) continue;

            drops[i].y++;
            if ((dodger_rand() % 6) == 0) drops[i].glyph = dodger_random_char();

            if (drops[i].y == DODGER_PLAYER_ROW && drops[i].x >= player_x - 1 && drops[i].x <= player_x + 1) {
                game_over = 1;
            }

            if (drops[i].y >= DODGER_PLAY_ROWS) {
                drops[i].active = 0;
                score += 5;
                continue;
            }

            draw_drop(vga, &drops[i]);
        }

        if (elite_hits_player(&elite, player_x)) game_over = 1;
        draw_elite(vga, &elite);
        draw_real_boss(vga, &boss);
        draw_boss_shots(vga, boss_shots);

        if (boss_warning > 0) draw_center_message(vga, "BOSS INCOMING", 0x0C);
        else if (boss_clear_message > 0) {
            draw_center_message(vga, "BOSS CLEARED", 0x0A);
            boss_clear_message--;
        }

        draw_player(vga, player_x);
        draw_status(vga, score, level, &boss);

        score++;
        responsive_wait(vga, &player_x, frame_delay);
    }

    if (game_over) {
        stop_sound();
        vga_write(vga, 10, 32, "GAME OVER", 0x0C);
        vga_write(vga, 12, 26, "Returning to KebabOS shell", 0x0F);
        play_game_over_sound();
        sleep_busy(1200);
    }

    dodger_running = 0;
    dodger_left_down = 0;
    dodger_right_down = 0;
    dodger_last_direction = 0;
    dodger_p_down = 0;
    dodger_m_down = 0;
    dodger_move_cooldown = 0;
    stop_sound();
}

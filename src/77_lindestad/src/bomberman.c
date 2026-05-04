#include <bomberman.h>

#include <interrupts.h>
#include <libc/stdint.h>
#include <libc/stdio.h>
#include <memory.h>
#include <pc_speaker.h>
#include <pit.h>
#include <terminal.h>

enum {
    MAP_WIDTH = 31,
    MAP_HEIGHT = 17,
    SCREEN_X = 2,
    SCREEN_Y = 3,
    TILE_WIDTH = 2,
    MAX_ENEMIES = 3,
    BOMB_FUSE_MS = 2100,
    BLAST_DURATION_MS = 420,
    ENEMY_START_DELAY_MS = 1800,
    ENEMY_STEP_MS = 430,
};

typedef struct {
    int32_t x;
    int32_t y;
    uint8_t alive;
    uint32_t last_move;
} Enemy;

typedef struct {
    int32_t x;
    int32_t y;
    uint8_t active;
    uint8_t exploding;
    uint32_t placed_at;
    uint32_t exploded_at;
} Bomb;

typedef struct {
    uint16_t frequency;
    uint16_t duration;
} SoundStep;

static const char level_template[MAP_HEIGHT][MAP_WIDTH + 1] = {
    "###############################",
    "#P..+..+.....+...+....+...+...#",
    "#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#",
    "#..+....+..+.....+..+....+....#",
    "#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#",
    "#....+....+...+....+....+..E..#",
    "#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#",
    "#..+....+....+....+....+......#",
    "#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#",
    "#.....+....+....+....+....+...#",
    "#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#",
    "#.E..+....+...+....+....+.....#",
    "#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#",
    "#...+....+.....+..+....+..E...#",
    "#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#",
    "#..+....+....+....+....+......#",
    "###############################",
};

static const SoundStep music_pattern[] = {
    {165, 120},
    {0, 35},
    {196, 120},
    {0, 35},
    {247, 120},
    {0, 35},
    {330, 120},
    {0, 35},
    {262, 120},
    {0, 35},
    {330, 120},
    {0, 35},
    {392, 120},
    {0, 35},
    {523, 120},
    {0, 35},
    {196, 120},
    {0, 35},
    {247, 120},
    {0, 35},
    {294, 120},
    {0, 35},
    {392, 120},
    {0, 35},
    {147, 120},
    {0, 35},
    {185, 120},
    {0, 35},
    {220, 120},
    {0, 35},
    {294, 120},
    {0, 35},
};

static const SoundStep start_effect[] = {
    {659, 70},
    {988, 90},
};

static const SoundStep bomb_effect[] = {
    {784, 60},
    {988, 60},
};

static const SoundStep blocked_effect[] = {
    {123, 55},
};

static const SoundStep explosion_effect[] = {
    {110, 80},
    {82, 80},
    {55, 110},
};

static const SoundStep death_effect[] = {
    {196, 120},
    {147, 140},
    {98, 180},
};

static const SoundStep victory_effect[] = {
    {523, 90},
    {659, 90},
    {784, 90},
    {1047, 150},
};

static char* map;
static int32_t player_x;
static int32_t player_y;
static Enemy enemies[MAX_ENEMIES];
static Bomb bomb;
static uint8_t blast_map[MAP_WIDTH * MAP_HEIGHT];
static uint32_t score;
static uint32_t crates_left;
static uint32_t enemies_left;
static uint8_t game_over;
static uint8_t victory;
static uint32_t rng_state = 0x21877u;
static uint32_t enemy_start_time;
static uint8_t audio_enabled = 1;
static const SoundStep* active_effect;
static uint32_t active_effect_length;
static uint32_t active_effect_index;
static uint32_t sound_step_started;
static uint32_t music_index;
static uint32_t current_frequency;

static char cell_at(int32_t x, int32_t y)
{
    return map[y * MAP_WIDTH + x];
}

static void set_cell(int32_t x, int32_t y, char value)
{
    map[y * MAP_WIDTH + x] = value;
}

static uint32_t rng_next(void)
{
    rng_state = rng_state * 1103515245u + 12345u;
    return rng_state;
}

static void set_output_frequency(uint32_t frequency)
{
    if (current_frequency == frequency) {
        return;
    }

    current_frequency = frequency;
    if (frequency == 0) {
        stop_sound();
    } else {
        play_sound(frequency);
    }
}

static void trigger_effect(const SoundStep* steps, uint32_t length)
{
    if (audio_enabled == 0) {
        return;
    }

    active_effect = steps;
    active_effect_length = length;
    active_effect_index = 0;
    sound_step_started = 0;
}

static void update_sound_sequence(uint32_t now, const SoundStep* steps, uint32_t length, uint32_t* index)
{
    if (sound_step_started == 0) {
        sound_step_started = now;
        set_output_frequency(steps[*index].frequency);
        return;
    }

    if (now - sound_step_started >= steps[*index].duration) {
        *index = (*index + 1) % length;
        sound_step_started = now;
        set_output_frequency(steps[*index].frequency);
    }
}

static void update_audio(void)
{
    uint32_t now = pit_get_ticks();

    if (audio_enabled == 0) {
        set_output_frequency(0);
        return;
    }

    if (active_effect != 0) {
        if (sound_step_started == 0) {
            sound_step_started = now;
            set_output_frequency(active_effect[active_effect_index].frequency);
            return;
        }

        if (now - sound_step_started >= active_effect[active_effect_index].duration) {
            active_effect_index++;
            sound_step_started = now;

            if (active_effect_index >= active_effect_length) {
                active_effect = 0;
                sound_step_started = 0;
                set_output_frequency(0);
            } else {
                set_output_frequency(active_effect[active_effect_index].frequency);
            }
        }
        return;
    }

    if (game_over != 0 || victory != 0) {
        set_output_frequency(0);
        return;
    }

    update_sound_sequence(now, music_pattern, sizeof(music_pattern) / sizeof(music_pattern[0]), &music_index);
}

static void set_game_over(void)
{
    if (game_over != 0 || victory != 0) {
        return;
    }

    game_over = 1;
    trigger_effect(death_effect, sizeof(death_effect) / sizeof(death_effect[0]));
}

static void draw_text(size_t x, size_t y, const char* text, uint8_t color)
{
    for (size_t i = 0; text[i] != 0; i++) {
        terminal_put_char_at(text[i], color, 0, x + i, y);
    }
}

static void draw_number(size_t x, size_t y, uint32_t value, uint8_t color)
{
    char digits[11];
    uint32_t length = 0;

    if (value == 0) {
        terminal_put_char_at('0', color, 0, x, y);
        return;
    }

    while (value > 0 && length < sizeof(digits)) {
        digits[length++] = (char)('0' + value % 10);
        value /= 10;
    }

    for (uint32_t i = 0; i < length; i++) {
        terminal_put_char_at(digits[length - 1 - i], color, 0, x + i, y);
    }
}

static uint8_t is_blast_cell(int32_t x, int32_t y)
{
    if (bomb.exploding == 0 || x < 0 || y < 0 || x >= MAP_WIDTH || y >= MAP_HEIGHT) {
        return 0;
    }

    return blast_map[y * MAP_WIDTH + x];
}

static void clear_blast_map(void)
{
    for (uint32_t i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {
        blast_map[i] = 0;
    }
}

static void mark_blast_cell(int32_t x, int32_t y)
{
    blast_map[y * MAP_WIDTH + x] = 1;
}

static uint8_t is_walkable(int32_t x, int32_t y)
{
    char cell = cell_at(x, y);
    return cell == '.' || cell == ' ';
}

static uint8_t is_enemy_at(int32_t x, int32_t y)
{
    for (uint32_t i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].alive != 0 && enemies[i].x == x && enemies[i].y == y) {
            return 1;
        }
    }

    return 0;
}

static void draw_map(void)
{
    for (uint32_t y = 0; y < MAP_HEIGHT; y++) {
        for (uint32_t x = 0; x < MAP_WIDTH; x++) {
            char cell = cell_at((int32_t)x, (int32_t)y);
            char glyph = ' ';
            uint8_t color = 7;

            if (cell == '#') {
                glyph = '#';
                color = 8;
            } else if (cell == '+') {
                glyph = '+';
                color = 6;
            } else if (cell == '.') {
                glyph = '.';
                color = 8;
            }

            if (bomb.active != 0 && bomb.x == (int32_t)x && bomb.y == (int32_t)y) {
                glyph = bomb.exploding != 0 ? '*' : 'o';
                color = bomb.exploding != 0 ? 14 : 4;
            } else if (is_blast_cell((int32_t)x, (int32_t)y) != 0) {
                glyph = '*';
                color = 14;
            }

            for (uint32_t i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].alive != 0 && enemies[i].x == (int32_t)x && enemies[i].y == (int32_t)y) {
                    glyph = 'E';
                    color = 12;
                }
            }

            if (player_x == (int32_t)x && player_y == (int32_t)y && game_over == 0) {
                glyph = '@';
                color = 10;
            }

            for (uint32_t column = 0; column < TILE_WIDTH; column++) {
                terminal_put_char_at(glyph, color, 0, SCREEN_X + x * TILE_WIDTH + column, SCREEN_Y + y);
            }
        }
    }
}

static void draw_ui(void)
{
    draw_text(2, 0, "assignment 6: bomberman-lite", 15);
    draw_text(2, 1, "wasd move | space bomb | r restart | q toggle sound", 7);
    draw_text(2, 22, "score:", 15);
    draw_number(9, 22, score, 10);
    draw_text(18, 22, "crates:", 15);
    draw_number(26, 22, crates_left, 14);
    draw_text(31, 22, "enemies:", 15);
    draw_number(40, 22, enemies_left, 12);

    if (victory != 0) {
        draw_text(48, 22, "level clear - press r", 10);
    } else if (game_over != 0) {
        draw_text(48, 22, "game over - press r", 12);
    } else if (audio_enabled == 0) {
        draw_text(48, 22, "sound muted", 8);
    } else if (bomb.active != 0 && bomb.exploding == 0) {
        draw_text(48, 22, "bomb armed", 4);
    } else {
        draw_text(48, 22, "survive and clear enemies", 7);
    }
}

static void draw_victory_screen(void)
{
    draw_text(27, 6, "level clear", 10);
    draw_text(24, 8, "all enemies defeated", 14);
    draw_text(24, 11, "final score:", 15);
    draw_number(37, 11, score, 10);
    draw_text(20, 15, "press r to start a new game", 7);
    draw_text(20, 17, "press q to toggle speaker output", 8);
}

static void render(void)
{
    terminal_clear();

    if (victory != 0) {
        draw_victory_screen();
        return;
    }

    draw_ui();
    draw_map();
}

static void reset_game(void)
{
    uint32_t enemy_index = 0;

    if (map == 0) {
        map = (char*)malloc(MAP_WIDTH * MAP_HEIGHT);
    }

    player_x = 1;
    player_y = 1;
    crates_left = 0;
    enemies_left = 0;
    score = 0;
    game_over = 0;
    victory = 0;
    bomb.active = 0;
    bomb.exploding = 0;
    enemy_start_time = pit_get_ticks();
    active_effect = 0;
    active_effect_index = 0;
    active_effect_length = 0;
    sound_step_started = 0;
    music_index = 0;
    set_output_frequency(0);
    clear_blast_map();

    for (uint32_t i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].alive = 0;
    }

    for (uint32_t y = 0; y < MAP_HEIGHT; y++) {
        for (uint32_t x = 0; x < MAP_WIDTH; x++) {
            char cell = level_template[y][x];

            if (cell == 'P') {
                player_x = (int32_t)x;
                player_y = (int32_t)y;
                cell = '.';
            } else if (cell == 'E' && enemy_index < MAX_ENEMIES) {
                enemies[enemy_index].x = (int32_t)x;
                enemies[enemy_index].y = (int32_t)y;
                enemies[enemy_index].alive = 1;
                enemies[enemy_index].last_move = pit_get_ticks();
                enemy_index++;
                enemies_left++;
                cell = '.';
            } else if (cell == '+') {
                crates_left++;
            }

            set_cell((int32_t)x, (int32_t)y, cell);
        }
    }

    trigger_effect(start_effect, sizeof(start_effect) / sizeof(start_effect[0]));
    render();
}

static void place_bomb(void)
{
    if (bomb.active != 0 || game_over != 0 || victory != 0) {
        return;
    }

    bomb.x = player_x;
    bomb.y = player_y;
    bomb.active = 1;
    bomb.exploding = 0;
    bomb.placed_at = pit_get_ticks();
    clear_blast_map();
    trigger_effect(bomb_effect, sizeof(bomb_effect) / sizeof(bomb_effect[0]));
}

static void try_move_player(int32_t dx, int32_t dy)
{
    int32_t next_x = player_x + dx;
    int32_t next_y = player_y + dy;

    if (game_over != 0 || victory != 0) {
        return;
    }

    if (is_walkable(next_x, next_y) == 0 || is_enemy_at(next_x, next_y) != 0) {
        trigger_effect(blocked_effect, sizeof(blocked_effect) / sizeof(blocked_effect[0]));
        return;
    }

    player_x = next_x;
    player_y = next_y;
}

static uint8_t blast_hits_wall_or_crate(int32_t x, int32_t y)
{
    char cell = cell_at(x, y);

    if (cell == '#') {
        return 1;
    }

    if (cell == '+') {
        set_cell(x, y, '.');
        if (crates_left > 0) {
            crates_left--;
        }
        score += 10;
        return 1;
    }

    return 0;
}

static void apply_blast_cell(int32_t x, int32_t y)
{
    if (player_x == x && player_y == y) {
        set_game_over();
    }

    for (uint32_t i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].alive != 0 && enemies[i].x == x && enemies[i].y == y) {
            enemies[i].alive = 0;
            if (enemies_left > 0) {
                enemies_left--;
            }
            score += 50;
        }
    }
}

static void explode_bomb(void)
{
    static const int32_t directions[4][2] = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1},
    };

    bomb.exploding = 1;
    bomb.exploded_at = pit_get_ticks();
    clear_blast_map();
    trigger_effect(explosion_effect, sizeof(explosion_effect) / sizeof(explosion_effect[0]));
    mark_blast_cell(bomb.x, bomb.y);
    apply_blast_cell(bomb.x, bomb.y);

    for (uint32_t dir = 0; dir < 4; dir++) {
        for (int32_t distance = 1; distance <= 2; distance++) {
            int32_t x = bomb.x + directions[dir][0] * distance;
            int32_t y = bomb.y + directions[dir][1] * distance;
            char cell = cell_at(x, y);

            if (cell == '#') {
                break;
            }
            mark_blast_cell(x, y);
            apply_blast_cell(x, y);
            if (blast_hits_wall_or_crate(x, y) != 0) {
                break;
            }
        }
    }
}

static void update_bomb(void)
{
    if (bomb.active == 0) {
        return;
    }

    uint32_t now = pit_get_ticks();
    if (bomb.exploding == 0 && now - bomb.placed_at >= BOMB_FUSE_MS) {
        explode_bomb();
    } else if (bomb.exploding != 0 && now - bomb.exploded_at >= BLAST_DURATION_MS) {
        bomb.active = 0;
        bomb.exploding = 0;
        clear_blast_map();
        if (game_over == 0 && enemies_left == 0) {
            victory = 1;
            trigger_effect(victory_effect, sizeof(victory_effect) / sizeof(victory_effect[0]));
        }
    }
}

static uint8_t clear_line_to_player(Enemy* enemy, int32_t* dx, int32_t* dy)
{
    *dx = 0;
    *dy = 0;

    if (enemy->x == player_x) {
        int32_t step = player_y > enemy->y ? 1 : -1;
        for (int32_t y = enemy->y + step; y != player_y; y += step) {
            if (is_walkable(enemy->x, y) == 0) {
                return 0;
            }
        }
        *dy = step;
        return 1;
    }

    if (enemy->y == player_y) {
        int32_t step = player_x > enemy->x ? 1 : -1;
        for (int32_t x = enemy->x + step; x != player_x; x += step) {
            if (is_walkable(x, enemy->y) == 0) {
                return 0;
            }
        }
        *dx = step;
        return 1;
    }

    return 0;
}

static void move_enemy(Enemy* enemy)
{
    static const int32_t directions[4][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
    };
    int32_t preferred_dx = 0;
    int32_t preferred_dy = 0;
    uint32_t start = rng_next() % 4;

    if (clear_line_to_player(enemy, &preferred_dx, &preferred_dy) != 0) {
        int32_t x = enemy->x + preferred_dx;
        int32_t y = enemy->y + preferred_dy;
        if (is_walkable(x, y) != 0 && is_blast_cell(x, y) == 0) {
            enemy->x = x;
            enemy->y = y;
            return;
        }
    }

    for (uint32_t i = 0; i < 4; i++) {
        uint32_t direction = (start + i) % 4;
        int32_t x = enemy->x + directions[direction][0];
        int32_t y = enemy->y + directions[direction][1];

        if (is_walkable(x, y) != 0 && is_blast_cell(x, y) == 0) {
            enemy->x = x;
            enemy->y = y;
            return;
        }
    }
}

static void update_enemies(void)
{
    uint32_t now = pit_get_ticks();

    if (now - enemy_start_time < ENEMY_START_DELAY_MS) {
        return;
    }

    for (uint32_t i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].alive == 0 || now - enemies[i].last_move < ENEMY_STEP_MS) {
            continue;
        }

        enemies[i].last_move = now;
        move_enemy(&enemies[i]);

        if (enemies[i].x == player_x && enemies[i].y == player_y) {
            set_game_over();
        }
    }
}

static void handle_key(char key)
{
    if (key == 'w') {
        try_move_player(0, -1);
    } else if (key == 'a') {
        try_move_player(-1, 0);
    } else if (key == 's') {
        try_move_player(0, 1);
    } else if (key == 'd') {
        try_move_player(1, 0);
    } else if (key == ' ') {
        place_bomb();
    } else if (key == 'r') {
        reset_game();
    } else if (key == 'q') {
        audio_enabled = audio_enabled == 0 ? 1 : 0;
        active_effect = 0;
        sound_step_started = 0;
        music_index = 0;
        set_output_frequency(0);
        if (audio_enabled == 0) {
            disable_speaker();
        }
    }
}

void run_bomberman(void)
{
    map = 0;
    keyboard_set_echo(0);
    reset_game();

    for (;;) {
        while (keyboard_has_key() != 0) {
            handle_key(keyboard_read_char());
        }

        update_bomb();
        update_enemies();

        if (is_blast_cell(player_x, player_y) != 0) {
            set_game_over();
        }

        update_audio();
        render();
        sleep_interrupt(50);
    }
}

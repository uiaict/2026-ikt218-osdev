#include "snake.h"
#include "../terminal.h"
#include "../song_player/song_player.h"
#include "../song_player/frequencies.h"

int player_x = 10;
int player_y = 10;

extern volatile int key_w;
extern volatile int key_a;
extern volatile int key_s;
extern volatile int key_d;
extern volatile int key_space;
extern volatile uint32_t tick;

uint32_t sound_tick = 0;
extern int speed;
static int sound_active = 0;

int dir_x = 1;
int dir_y = 0;
int game_over = 0;
int food_x = 20;
int food_y = 10;
int score = 0;

typedef struct {
    int x;
    int y;
} Segment;

#define MAX_SNAKE_LENGTH 100

Segment snake[MAX_SNAKE_LENGTH];
int snake_length = 1;

int random(int max)
{
    return tick % max;
}

void int_to_str(int num, char* str)
{
    int i = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (num > 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    str[i] = '\0';

    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char tmp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = tmp;
    }
}

// SPAWN FOOD
void spawn_food()
{
    food_x = (random(78) + 1);
    food_y = (random(23) + 2);
}

void game_init()
{
    score = 0;
    snake_length = 3;

    for (int i = 0; i < snake_length; i++) {
        snake[i].x = 10 - i;
        snake[i].y = 10;
    }

    dir_x = 1;
    dir_y = 0;
    game_over = 0;

    speed = 200; // Reset speed when the game restarts
    spawn_food();
}

void game_update()
{
    if (sound_active)
    {
        if (tick - sound_tick > 5)
        {
            stop_sound();
            sound_active = 0;
        }
    }

    if (game_over)
    {
        // Restart if player presses space
        if (key_space)
        {
            game_init();
        }

        // Reset input regardless
        key_space = 0;
        return;
    }

    // Direction control
    if (key_w) { dir_x = 0; dir_y = -1; }
    else if (key_s) { dir_x = 0; dir_y = 1; }
    else if (key_a) { dir_x = -1; dir_y = 0; }
    else if (key_d) { dir_x = 1; dir_y = 0; }

    key_w = key_a = key_s = key_d = 0;

    // Move body
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Move head
    snake[0].x += dir_x;
    snake[0].y += dir_y;

    if (snake[0].x == food_x &&
        snake[0].y == food_y)
    {
        if (snake_length < MAX_SNAKE_LENGTH) {
            snake_length++;
        }

        score++;
        spawn_food();

        play_sound(D7); // Play sound when food is eaten
        sound_tick = tick;
        sound_active = 1;

        // Increase speed
        if (speed > 50)
            speed -= 10;
    }

    // Wall collision
    if (snake[0].x >= 79 || snake[0].x <= 0 ||
        snake[0].y >= 24 || snake[0].y <= 1)
    {
        game_over = 1;

        play_sound(C3); // Play sound on collision (game over)
        sound_tick = tick;
        sound_active = 1;
        return;
    }

    // Self collision
    for (int i = 1; i < snake_length; i++) {
        if (snake[0].x == snake[i].x &&
            snake[0].y == snake[i].y)
        {
            game_over = 1;

            play_sound(C3); // Play sound on collision (game over)
            sound_tick = tick;
            sound_active = 1;
            return;
        }
    }
}

void game_draw()
{
    terminal_clear();
    draw_border();

    char score_str[10];
    int_to_str(score, score_str);

    // Write "Score: "
    const char* label = "Score: ";
    int x = 0;
    int y = 0;

    // Print label
    for (int i = 0; label[i] != '\0'; i++) {
        terminal_put_at(label[i], 0x0F, x + i, y);
    }

    // Print number
    for (int i = 0; score_str[i] != '\0'; i++) {
        terminal_put_at(score_str[i], 0x0F, x + 7 + i, y);
    }

    terminal_put_at('*', 0x0C, food_x, food_y);

    for (int i = 0; i < snake_length; i++) {
        terminal_put_at(
            i == 0 ? 'O' : 'o',
            0x0A,
            snake[i].x,
            snake[i].y
        );
    }

    if (game_over) {
        const char* msg = "GAME OVER - Press SPACE to restart";
        int x = 24;
        int y = 12;

        for (int i = 0; msg[i] != '\0'; i++) {
            terminal_put_at(msg[i], 0x0F, x + i, y);
        }
    }
}
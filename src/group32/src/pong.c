#include "pong.h"
#include "screen.h"
#include "keyboard.h"
#include "pit.h"

#define WIDTH 80
#define HEIGHT 25

#define LEFT_PADDLE_X 3
#define RIGHT_PADDLE_X 76
#define PADDLE_HEIGHT 5

#define MIN_Y 2
#define MAX_Y 23

static void draw_pong(
    int left_paddle_y,
    int right_paddle_y,
    int ball_x,
    int ball_y,
    int left_score,
    int right_score
) {
    screen_clear();

    screen_write("2 PLAYER PONG | Left: W/S | Right: Arrow Up/Down | Q quit\n");
    screen_write("Left score: ");
    screen_write_dec(left_score);
    screen_write("   Right score: ");
    screen_write_dec(right_score);
    screen_write("\n");

    for (int y = 2; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 || x == WIDTH - 1) {
                screen_putc('|');
            }
            else if (y == 2 || y == HEIGHT - 1) {
                screen_putc('-');
            }
            else if (x == LEFT_PADDLE_X &&
                     y >= left_paddle_y &&
                     y < left_paddle_y + PADDLE_HEIGHT) {
                screen_putc('#');
            }
            else if (x == RIGHT_PADDLE_X &&
                     y >= right_paddle_y &&
                     y < right_paddle_y + PADDLE_HEIGHT) {
                screen_putc('#');
            }
            else if (x == ball_x && y == ball_y) {
                screen_putc('O');
            }
            else {
                screen_putc(' ');
            }
        }
    }
}

static void reset_ball(int* ball_x, int* ball_y, int* ball_dx, int* ball_dy) {
    *ball_x = WIDTH / 2;
    *ball_y = HEIGHT / 2;
    *ball_dx = -(*ball_dx);
    *ball_dy = 1;
}

void play_pong_game(void) {
    int left_paddle_y = 10;
    int right_paddle_y = 10;

    int ball_x = WIDTH / 2;
    int ball_y = HEIGHT / 2;
    int ball_dx = -1;
    int ball_dy = 1;

    int left_score = 0;
    int right_score = 0;

    while (1) {
        draw_pong(
            left_paddle_y,
            right_paddle_y,
            ball_x,
            ball_y,
            left_score,
            right_score
        );

        char key = keyboard_read_char();

        while (key != 0) {
            if (key == 'w' || key == 'W') {
                if (left_paddle_y > MIN_Y + 1) {
                    left_paddle_y--;
                }
            }
            else if (key == 's' || key == 'S') {
                if (left_paddle_y + PADDLE_HEIGHT < MAX_Y) {
                    left_paddle_y++;
                }
            }
            else if (key == KEY_ARROW_UP) {
                if (right_paddle_y > MIN_Y + 1) {
                    right_paddle_y--;
                }
            }
            else if (key == KEY_ARROW_DOWN) {
                if (right_paddle_y + PADDLE_HEIGHT < MAX_Y) {
                    right_paddle_y++;
                }
            }
            else if (key == 'q' || key == 'Q') {
                screen_clear();
                screen_write("Returning to menu...\n");
                pit_sleep_ms(500);
                return;
            }

            key = keyboard_read_char();
        }

        int next_x = ball_x + ball_dx;
        int next_y = ball_y + ball_dy;

        if (next_y <= MIN_Y + 1 || next_y >= MAX_Y - 1) {
            ball_dy = -ball_dy;
            next_y = ball_y + ball_dy;
        }

        if (next_x == LEFT_PADDLE_X + 1) {
            if (next_y >= left_paddle_y &&
                next_y < left_paddle_y + PADDLE_HEIGHT) {
                ball_dx = 1;
                next_x = ball_x + ball_dx;

                if (next_y == left_paddle_y) {
                    ball_dy = -1;
                }
                else if (next_y == left_paddle_y + PADDLE_HEIGHT - 1) {
                    ball_dy = 1;
                }
            }
        }

        if (next_x == RIGHT_PADDLE_X - 1) {
            if (next_y >= right_paddle_y &&
                next_y < right_paddle_y + PADDLE_HEIGHT) {
                ball_dx = -1;
                next_x = ball_x + ball_dx;

                if (next_y == right_paddle_y) {
                    ball_dy = -1;
                }
                else if (next_y == right_paddle_y + PADDLE_HEIGHT - 1) {
                    ball_dy = 1;
                }
            }
        }

        ball_x = next_x;
        ball_y = next_y;

        if (ball_x <= 1) {
            right_score++;
            reset_ball(&ball_x, &ball_y, &ball_dx, &ball_dy);
            pit_sleep_ms(500);
        }

        if (ball_x >= WIDTH - 2) {
            left_score++;
            reset_ball(&ball_x, &ball_y, &ball_dx, &ball_dy);
            pit_sleep_ms(500);
        }

        if (left_score >= 5 || right_score >= 5) {
            screen_clear();

            if (left_score > right_score) {
                screen_write("Left player wins!\n");
            }
            else {
                screen_write("Right player wins!\n");
            }

            screen_write("\nFinal score: ");
            screen_write_dec(left_score);
            screen_write(" - ");
            screen_write_dec(right_score);

            screen_write("\n\nPress any key to return to menu...");
            keyboard_get_char();
            return;
        }

        pit_sleep_ms(90);
    }
}
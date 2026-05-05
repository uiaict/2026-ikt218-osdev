#include "pong.h"
#include "screen.h"
#include "pit.h"
#include "keyboard.h"

// ==============================
// Pong game (text mode)
//
// Simple Pong clone drawn directly
// into VGA text memory, using the
// PIT for timing and keyboard for
// paddle control.
// ==============================

#define VGA ((unsigned short*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define PONG_WIDTH  80
#define PONG_HEIGHT 25
#define PADDLE_H 4
#define WIN_SCORE 5
#define COLOR 0x0F00        // Bright white on black (attribute word)

// Paddle representation
typedef struct {
    int x;
    int y;
    int h;
} Paddle;

// Ball representation
typedef struct {
    int x;
    int y;
    int vx;
    int vy;
} Ball;

// Game state
typedef struct {
    Paddle left;
    Paddle right;
    Ball ball;
    int score_left;
    int score_right;
    int game_over;
} PongGame;

// Draw a single character at (x, y) in VGA text mode
static void draw_char_at(int x, int y, char c) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) {
        return;
    }
    VGA[y * VGA_WIDTH + x] = COLOR | (unsigned short)c;
}

// Draw a null-terminated string at (x, y)
static void draw_string_at(int x, int y, const char* s) {
    while (*s && x < VGA_WIDTH) {
        draw_char_at(x, y, *s);
        x++;
        s++;
    }
}

// Draw a positive integer at (x, y)
static void draw_number_at(int x, int y, int n) {
    char buf[12];
    int i = 0;

    if (n == 0) {
        draw_char_at(x, y, '0');
        return;
    }

    // Build digits in reverse order
    while (n > 0 && i < 11) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }

    // Draw digits in correct order
    while (i > 0) {
        draw_char_at(x++, y, buf[--i]);
    }
}

// Clear entire screen to spaces
static void clear_screen_pong(void) {
    unsigned short blank = COLOR | ' ';
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA[y * VGA_WIDTH + x] = blank;
        }
    }
}

// Draw top/bottom borders and center divider
static void draw_border(void) {
    for (int x = 0; x < PONG_WIDTH; x++) {
        draw_char_at(x, 1, '-');
        draw_char_at(x, PONG_HEIGHT - 1, '-');
    }

    for (int y = 2; y < PONG_HEIGHT - 1; y++) {
        draw_char_at(PONG_WIDTH / 2, y, ':');
    }
}

// Draw a paddle as a vertical bar
static void draw_paddle(Paddle* p) {
    for (int i = 0; i < p->h; i++) {
        draw_char_at(p->x, p->y + i, '|');
    }
}

// Draw the ball
static void draw_ball(Ball* b) {
    draw_char_at(b->x, b->y, 'O');
}

// Reset ball to center and set direction based on total score
static void reset_ball(PongGame* g) {
    g->ball.x = PONG_WIDTH / 2;
    g->ball.y = PONG_HEIGHT / 2;
    g->ball.vx = ((g->score_left + g->score_right) % 2 == 0) ? 1 : -1;
    g->ball.vy = 1;
}

// Initialize paddles, scores and ball
static void init_game(PongGame* g) {
    g->left.x = 2;
    g->left.y = 10;
    g->left.h = PADDLE_H;

    g->right.x = 77;
    g->right.y = 10;
    g->right.h = PADDLE_H;

    g->score_left = 0;
    g->score_right = 0;
    g->game_over = 0;

    reset_ball(g);
}

// Keep paddles within the play field
static void clamp_paddles(PongGame* g) {
    if (g->left.y < 2) {
        g->left.y = 2;
    }
    if (g->left.y + g->left.h > PONG_HEIGHT - 1) {
        g->left.y = PONG_HEIGHT - 1 - g->left.h;
    }

    if (g->right.y < 2) {
        g->right.y = 2;
    }
    if (g->right.y + g->right.h > PONG_HEIGHT - 1) {
        g->right.y = PONG_HEIGHT - 1 - g->right.h;
    }
}

// Simple AI for right paddle: track ball vertically
static void update_ai(PongGame* g) {
    int center = g->right.y + g->right.h / 2;

    if (g->ball.y < center) {
        g->right.y--;
    } else if (g->ball.y > center) {
        g->right.y++;
    }
}

// Update ball position, handle collisions and scoring
static void update_ball(PongGame* g) {
    g->ball.x += g->ball.vx;
    g->ball.y += g->ball.vy;

    // Bounce off top/bottom borders
    if (g->ball.y <= 2 || g->ball.y >= PONG_HEIGHT - 2) {
        g->ball.vy = -g->ball.vy;
    }

    // Collision with left paddle
    if (g->ball.x == g->left.x + 1 &&
        g->ball.y >= g->left.y &&
        g->ball.y < g->left.y + g->left.h) {
        g->ball.vx = 1;
    }

    // Collision with right paddle
    if (g->ball.x == g->right.x - 1 &&
        g->ball.y >= g->right.y &&
        g->ball.y < g->right.y + g->right.h) {
        g->ball.vx = -1;
    }

    // Ball passed left side: right player scores
    if (g->ball.x < 0) {
        g->score_right++;
        reset_ball(g);
    }

    // Ball passed right side: left player scores
    if (g->ball.x > PONG_WIDTH - 1) {
        g->score_left++;
        reset_ball(g);
    }

    // Check for game over
    if (g->score_left >= WIN_SCORE || g->score_right >= WIN_SCORE) {
        g->game_over = 1;
    }
}

// Draw full game frame
static void draw_game(PongGame* g) {
    clear_screen_pong();

    // Score and controls
    draw_string_at(2, 0, "PONG  Left:");
    draw_number_at(14, 0, g->score_left);
    draw_string_at(18, 0, "Right:");
    draw_number_at(25, 0, g->score_right);
    draw_string_at(35, 0, "W/S = move, M = menu");

    draw_border();
    draw_paddle(&g->left);
    draw_paddle(&g->right);
    draw_ball(&g->ball);

    // Game over message
    if (g->game_over) {
        if (g->score_left > g->score_right) {
            draw_string_at(22, 12, "You win! Press M to return.");
        } else {
            draw_string_at(18, 12, "Computer wins! Press M to return.");
        }
    }
}

// Main Pong loop
void run_pong(void) {
    PongGame game;
    init_game(&game);

    while (1) {
        // Handle keyboard input
        while (keyboard_has_char()) {
            char c = keyboard_get_char();

            // Return to menu
            if (c == 'm' || c == 'M') {
                return;
            }

            // Player paddle control (only while game active)
            if (!game.game_over) {
                if (c == 'w' || c == 'W') {
                    game.left.y--;
                }
                if (c == 's' || c == 'S') {
                    game.left.y++;
                }
            }
        }

        // Update game state
        if (!game.game_over) {
            update_ai(&game);
            clamp_paddles(&game);
            update_ball(&game);
        }

        // Render frame
        draw_game(&game);

        // Small delay to control game speed
        sleep_interrupt(40);
    }
}
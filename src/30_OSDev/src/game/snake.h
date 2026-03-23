#ifndef SNAKE_H
#define SNAKE_H
extern int player_x;
extern int player_y;


void game_init();
void game_update();
void game_draw();
void spawn_food();
int random(int max);

#endif
